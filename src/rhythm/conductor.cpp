#include "rhythm/conductor.hpp"

namespace vsrg {
Conductor::Conductor(AudioManager* audio_manager, Audio* audio,
                     std::vector<TimingPoint> timing_points)
    : audio_manager(audio_manager), audio(audio), timing_points(std::move(timing_points)) {
    song_duration = audio->get_duration();

    LatencyInfo latency = audio_manager->get_latency_info();
    if (latency.valid) {
        cached_latency = (float)latency.period_size_in_milliseconds / 1000.0f;
    }

    if (!this->timing_points.empty()) {
        this->current_point = &this->timing_points[0];
        this->current_point_index = 0;
    } else {
        this->current_point = nullptr;
    }

    audio->set_playback_rate(playback_rate);
}

Conductor::~Conductor() {
    stop();
}

void Conductor::play() {
    if (audio_manager && audio) {
        audio_manager->play_audio(audio);
    }
}

void Conductor::stop() {
    if (audio_manager && audio) {
        audio_manager->stop_audio(audio);
    }
}

void Conductor::seek(float time_in_seconds) {
    if (!audio || !audio->is_initialized()) return;

    audio->set_position(time_in_seconds);
    song_position = time_in_seconds;

    int found_index = 0;
    for (size_t i = 0; i < timing_points.size(); ++i) {
        if (song_position >= timing_points[i].time) {
            found_index = i;
        } else {
            break;
        }
    }

    current_point_index = found_index;
    current_point = &timing_points[current_point_index];
    updateBPM();
}

void Conductor::update(float delta_time) {
    if (!audio || audio->get_paused()) return;

    float hardware_pos = audio->get_position();
    if (hardware_pos != last_hardware_position) {
        song_position = hardware_pos - cached_latency;
        last_hardware_position = hardware_pos;
    } else {
        song_position += delta_time * playback_rate;
    }

    if (song_position < 0) song_position = 0;

    if (song_duration <= 0.0f) {
        float duration = audio->get_duration();
        if (duration > 0.0f) {
            song_duration = duration;
        }
    } else {
        if (song_position >= song_duration) stop();
    }

    if (current_point) {
        // this is a magic number that i am not sure is right but we will find out later :p
        double seconds_per_subdivision =
            (240.0 / current_point->bpm) / (double)current_point->denominator;
        if (seconds_per_subdivision > 0) {
            double time_elapsed = (double)song_position - (double)current_point->time;
            double total_steps_elapsed = time_elapsed / seconds_per_subdivision;

            current_step = (int)std::floor(total_steps_elapsed);
            current_beat = (int)std::floor(total_steps_elapsed / (double)current_point->nominator);

            if (current_step != last_step) {
                // todo: implement events on this (can be for visuals, modcharts, anything)
                last_step = current_step;
            }
            if (current_beat != last_beat) {
                // todo: implement events on this (can be for visuals, modcharts, anything)
                last_beat = current_beat;
            }
        }
    }

    if (current_point_index + 1 < timing_points.size()) {
        if (song_position >= timing_points[current_point_index + 1].time) {
            current_point_index++;
            current_point = &timing_points[current_point_index];
            updateBPM();
        }
    }
}

float Conductor::get_bpm_at_time(float time) {
    if (timing_points.empty()) {
        return 120.0f;  // fallback bpm
    }

    size_t found_index = 0;
    for (size_t i = 0; i < timing_points.size(); ++i) {
        if (time >= timing_points[i].time) {
            found_index = i;
        } else {
            break;
        }
    }

    return timing_points[found_index].bpm;
}

void Conductor::updateBPM() {
    // event for bpm changes eventually
}
}  // namespace vsrg