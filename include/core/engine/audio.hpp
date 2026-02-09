#pragma once

#include <miniaudio.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "public/engineContext.hpp"


namespace vsrg {
// i will use this later to sort sound groups, for now does nothing
enum class AudioType { SoundEffect, Music, Miscellaneous };

struct LatencyInfo {
    ma_uint32 period_size_in_frames;
    ma_uint32 period_size_in_milliseconds;
    ma_uint32 sample_rate;
    bool valid;
};

class Audio {
public:
    Audio() : initialized(false) {}
    ~Audio() {
        if (initialized) {
            ma_sound_uninit(&sound);
        }
    }

    ma_sound *get_sound() { return &sound; }
    bool is_initialized() { return initialized; }

    void set_paused(bool paused) { is_paused = paused; }
    bool get_paused() { return is_paused; }

    void set_volume(float _volume) {
        if (initialized) {
            ma_sound_set_volume(&sound, _volume);
        }
        volume = _volume;
    };
    float get_volume() { return volume; }

    void set_looping(bool _looping) {
        if (initialized) {
            ma_sound_set_looping(&sound, _looping);
        }
        looping = _looping;
    };
    float get_looping() { return looping; }

    void set_playback_rate(float _playback_rate) {
        if (initialized) {
            // TODO: change this away from pitch so it doesnt sound like chipmunks
            ma_sound_set_pitch(&sound, _playback_rate);
        }
        playback_rate = _playback_rate;
    };
    float get_playback_rate() { return playback_rate; }
    float get_duration() {
        if (!initialized) return 0.0f;

        ma_result result;
        ma_uint64 lengthInFrames;
        ma_uint32 sampleRate;

        result = ma_sound_get_length_in_pcm_frames(&sound, &lengthInFrames);
        if (result != MA_SUCCESS) {
            return 0.0f;
        }

        ma_sound_get_data_format(&sound, NULL, NULL, &sampleRate, NULL, 0);
        if (sampleRate == 0) return 0.0f;

        return (float)lengthInFrames / (float)sampleRate;
    }
    float get_position() {
        if (!initialized) return 0.0f;

        ma_result result;
        ma_uint64 cursorInFrames;
        ma_uint32 sampleRate;

        result = ma_sound_get_cursor_in_pcm_frames(&sound, &cursorInFrames);
        if (result != MA_SUCCESS) {
            return 0.0f;
        }

        ma_sound_get_data_format(&sound, NULL, NULL, &sampleRate, NULL, 0);
        if (sampleRate == 0) return 0.0f;

        return (float)cursorInFrames / (float)sampleRate;
    }
    void set_position(float time_in_seconds) {
        if (!initialized) return;

        ma_uint32 sampleRate;

        ma_sound_get_data_format(&sound, NULL, NULL, &sampleRate, NULL, 0);
        ma_uint64 frameIndex = (ma_uint64)(time_in_seconds * sampleRate);
        ma_sound_seek_to_pcm_frame(&sound, frameIndex);
    }

private:
    bool initialized;
    ma_sound sound;

    float volume = 1.0f;
    float playback_rate = 1.0f;

    bool is_paused = true;
    bool looping = false;

    friend class AudioManager;
};

struct AudioResult {
    ma_result status;
    Audio *audio;
};

class AudioManager {
public:
    AudioManager(EngineContext *engine_context);
    ~AudioManager();

    bool is_initialized() const { return initialized; }

    AudioResult load_audio(std::string file_path);
    AudioResult unload_audio(Audio *audio);

    AudioResult play_audio(Audio *audio);
    AudioResult stop_audio(Audio *audio);

    void stop_all_audios();
    void unload_all_audios();

    LatencyInfo get_latency_info();

private:
    EngineContext *engine_context;
    bool initialized = false;
    ma_engine engine;

    std::vector<Audio *> loaded_audios;
};
}  // namespace vsrg