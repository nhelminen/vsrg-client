#pragma once

#include "core/screen.hpp"
#include "core/font.hpp"
#include "core/ui/textComponent.hpp"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace vsrg {
    class EngineContext;

    class DebugScreen : public Screen {
    public:
        DebugScreen(EngineContext* engine_context);
        ~DebugScreen() override;
        
        void update(float delta_time) override;
        void render() override;
    private:
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint shader_program = 0;
        FontManager font_manager;
        Font font;
        TextComponent text_component;
    };
}