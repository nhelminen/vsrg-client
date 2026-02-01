#pragma once

#include <iostream>
#include <miniaudio.h>

namespace vsrg {
    class AudioManager {
    public:
        AudioManager();
        ~AudioManager();

        bool is_initialized() const { return initialized; }
    private:
        bool initialized = false;
        ma_engine engine;
    };
}