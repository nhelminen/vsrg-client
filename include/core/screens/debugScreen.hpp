#pragma once

#include "core/engine/screen.hpp"
#include "core/ui/font.hpp"

#include "core/engine/plugin.hpp"

#include "core/ui/textComponent.hpp"
#include "core/ui/spriteComponent.hpp"

#include "rhythm/conductor.hpp"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <iomanip>
#include <sstream>
#include <string>

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

        Conductor* conductor;
        Audio* loaded_audio;

        TextComponent text_component;
        SpriteComponent sprite_component;

        IGamePlugin* gameplay_plugin;
    };
}