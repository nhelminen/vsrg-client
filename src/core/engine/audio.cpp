#include "core/engine/audio.hpp"

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

namespace vsrg {
AudioManager::AudioManager(EngineContext* engine_context) : engine_context(engine_context) {
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine: " << result << std::endl;
        initialized = false;
        return;
    }

    initialized = true;
}

AudioManager::~AudioManager() {
    stop_all_audios();
    unload_all_audios();

    if (initialized) {
        ma_engine_uninit(&engine);
    }
}

AudioResult AudioManager::load_audio(std::string file_path) {
    Audio* audio = new Audio();

    ma_uint32 flags = MA_SOUND_FLAG_STREAM;
    ma_result result =
        ma_sound_init_from_file(&engine, file_path.c_str(), flags, NULL, NULL, &audio->sound);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to load sound from file: " << file_path << " Error: " << result
                  << std::endl;
        delete audio;
        return {result, nullptr};
    }

    audio->initialized = true;
    loaded_audios.push_back(audio);

    return {result, audio};
}

AudioResult AudioManager::unload_audio(Audio* audio) {
    if (!audio)
        return {MA_INVALID_ARGS, nullptr};

    auto it = std::find(loaded_audios.begin(), loaded_audios.end(), audio);
    if (it != loaded_audios.end()) {
        loaded_audios.erase(it);
    }

    delete audio;
    return {MA_SUCCESS, nullptr};
}

AudioResult AudioManager::play_audio(Audio* audio) {
    if (!audio)
        return {MA_INVALID_ARGS, nullptr};
    if (!audio->is_initialized())
        return {MA_INVALID_OPERATION, nullptr};

    if (!audio->get_paused())
        return {MA_INVALID_OPERATION, audio};

    ma_result result = ma_sound_start(audio->get_sound());
    if (result == MA_SUCCESS) {
        audio->set_paused(false);
    }

    return {result, audio};
}

AudioResult AudioManager::stop_audio(Audio* audio) {
    if (!audio)
        return {MA_INVALID_ARGS, nullptr};
    if (!audio->is_initialized())
        return {MA_INVALID_OPERATION, nullptr};

    if (audio->get_paused())
        return {MA_INVALID_OPERATION, audio};

    ma_result result = ma_sound_stop(audio->get_sound());
    if (result == MA_SUCCESS) {
        audio->set_paused(true);
    }

    return {result, audio};
}

void AudioManager::stop_all_audios() {
    for (Audio* audio : loaded_audios) {
        stop_audio(audio);
    }
}

void AudioManager::unload_all_audios() {
    for (Audio* audio : loaded_audios) {
        if (audio) {
            delete audio;
        }
    }
    loaded_audios.clear();
}

LatencyInfo AudioManager::get_latency_info() {
    LatencyInfo info = {};
    info.valid = false;

    if (!initialized) {
        return info;
    }

    ma_device* pDevice = ma_engine_get_device(&engine);

    if (pDevice) {
        info.period_size_in_frames = pDevice->playback.internalPeriodSizeInFrames;
        info.sample_rate = pDevice->sampleRate;
        info.period_size_in_milliseconds = (info.period_size_in_frames * 1000) / info.sample_rate;
        info.valid = true;
    }

    return info;
}
} // namespace vsrg