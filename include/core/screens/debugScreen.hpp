#pragma once

#include "core/engine/screen.hpp"
#include "core/ui/font.hpp"

#include "core/ui/textComponent.hpp"
#include "core/ui/spriteComponent.hpp"

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
        FontManager font_manager;
        Font font;

        TextComponent text_component;
        SpriteComponent sprite_component;
    };
}