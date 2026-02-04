#pragma once

#include <vector>
#include "core/engine/audio.hpp"
#include "core/engine/shader.hpp"

namespace vsrg
{
    struct TimingPoint {
        float time;
        double bpm;

        int nominator;
        int denominator;
    };

    class Conductor
    {
    public:
        Conductor(AudioManager* audio_manager, Audio* audio, std::vector<TimingPoint> timing_points);
        ~Conductor();

        void play();
        void stop();

        void update(float delta_time);
        void seek(float time_in_seconds);
        
        float get_bpm() { 
            if (current_point != nullptr)
                return current_point->bpm;
            return -1.0f; 
        }
        glm::vec2 get_time_signature() { 
            if (current_point != nullptr)
                return glm::vec2(
                    current_point->nominator, 
                    current_point->denominator
                );
            return glm::vec2(4, 4); 
        }
        
        float get_song_position() { return song_position; }
        float get_song_duration() { return song_duration; }
        float get_playback_rate() { return playback_rate; }

        int get_beat() { return current_beat; }
        int get_step() { return current_step; }
    private:
        Audio* audio;
        AudioManager* audio_manager;
        TimingPoint* current_point;

        float playback_rate = 1.0f;
        float song_position = 0.0f;
        float song_duration = 0.0f;

        float last_hardware_position = 0.0f;
        float cached_latency = 0.0f;

        int current_beat = 0;
        int current_step = 0;

        int last_step = -1;
        int last_beat = -1;
        
        size_t current_point_index = 0;
        std::vector<TimingPoint> timing_points;
        
        void updateBPM();
    };
}