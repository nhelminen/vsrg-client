#pragma once

#include "core/screen.hpp"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace vsrg {
    class EngineContext;

    class TestScreen : public Screen {
    public:
        TestScreen(EngineContext* engine_context);
        ~TestScreen() override;
        
        void update(float delta_time) override;
        void render() override;
    private:
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint shader_program = 0;
    };
}