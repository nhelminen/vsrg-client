#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <miniaudio.h>

namespace vsrg
{
    // i will use this later to sort sound groups, for now does nothing
    enum class AudioType {
        SoundEffect,
        Music,
        Miscellaneous
    };

    struct LatencyInfo {
        ma_uint32 period_size_in_frames;
        ma_uint32 period_size_in_milliseconds;
        ma_uint32 sample_rate;
        bool valid;
    };

    class Audio
    {
    public:
        Audio() : initialized(false) {}
        ~Audio() {
            if (initialized)
            {
                ma_sound_uninit(&sound);
            }
        }

        ma_sound *get_sound() { return &sound; }
        bool is_initialized() { return initialized; }

        void set_paused(bool paused) { is_paused = paused; }
        bool get_paused() { return is_paused; }

        void set_volume(float _volume) {
            if (initialized)
            {
                ma_sound_set_volume(&sound, _volume);
            }
            volume = _volume;
        };
        float get_volume() { return volume; }

        void set_looping(bool _looping) {
            if (initialized)
            {
                ma_sound_set_looping(&sound, _looping);
            }
            looping = _looping;
        };
        float get_looping() { return looping; }

        void set_playback_rate(float _playback_rate) {
            if (initialized)
            {
                // TODO: change this away from pitch so it doesnt sound like chipmunks
                ma_sound_set_pitch(&sound, _playback_rate);
            }
            playback_rate = _playback_rate;
        };
        float get_playback_rate() { return playback_rate; }
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
        Audio* audio;
    };

    class AudioManager
    {
    public:
        AudioManager();
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
        bool initialized = false;
        ma_engine engine;

        std::vector<Audio *> loaded_audios;
    };
}