#pragma once

#include "core/engine/screen.hpp"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "string"

namespace vsrg {
    class EngineContext;

    class InitScreen : public Screen {
    public:
        InitScreen(EngineContext* engine_context);
        ~InitScreen() override;
        
        void update(float delta_time) override;
        void render() override;
    };
}