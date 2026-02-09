#pragma once

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "core/debug.hpp"
#include "core/engine/audio.hpp"
#include "core/engine/plugin.hpp"
#include "core/engine/screen.hpp"
#include "core/ui/font.hpp"
#include "public/engineContext.hpp"

using Clock = std::chrono::high_resolution_clock;

namespace vsrg {
class Client {
public:
    Client(int screen_width = 1280, int screen_height = 720);
    ~Client();

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    Client(Client&&) = delete;
    Client& operator=(Client&&) = delete;

    void start();
    bool is_initialized() const { return gl_initialized; }
    float get_delta_time() const { return delta_time; }

    int get_screen_width() const { return SCREEN_WIDTH; }
    int get_screen_height() const { return SCREEN_HEIGHT; }

    EngineContext* get_engine_context() const { return engine_context; }

private:
    Clock::time_point last_time;
    float delta_time;

    bool gl_initialized = false;
    bool should_close = false;

    SDL_Window* window;
    SDL_GLContext gl_context;

    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;

    EngineContext* engine_context = nullptr;
};
}  // namespace vsrg