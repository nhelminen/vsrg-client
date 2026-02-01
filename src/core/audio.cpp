#include "core/audio.hpp"

#define MINIAUDIO_IMPLEMENTATION // i cant do it in header cause it would break it all
#include <miniaudio.h>

namespace vsrg {
    AudioManager::AudioManager() {
        ma_result result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) {
            std::cerr << "Failed to initialize audio engine: " << result << std::endl;
            initialized = false;
        }

        initialized = true;
        
    }

    AudioManager::~AudioManager() {
        if (initialized) {
            ma_engine_uninit(&engine);
        }
    }
}