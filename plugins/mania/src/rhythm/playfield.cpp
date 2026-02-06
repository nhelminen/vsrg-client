#include "rhythm/playfield.hpp"
#include "core/debug.hpp"

namespace mania {
    Playfield::Playfield(vsrg::EngineContext* ctx, int key_count, glm::vec4 background_color) 
        : vsrg::SolidComponent(ctx, background_color), engine_context(ctx), key_count(key_count) {
        
        VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO, "creating playfield with " + std::to_string(key_count) + " keys");

        float screen_width = static_cast<float>(ctx->get_screen_width());
        float screen_height = static_cast<float>(ctx->get_screen_height());

        float strum_width = 96.0f;
        float strum_spacing = 8.0f;
        float total_width = (strum_width * key_count) + (strum_spacing * (key_count - 1));
        
        float start_x = (screen_width - total_width) / 2.0f;
        float strum_y = screen_height - 128.0f;

        setPosition(glm::vec2(start_x - strum_spacing, 0.0f));
        setSize(glm::vec2(total_width + (strum_spacing * 2.0f), screen_height));

        for (int i = 0; i < key_count; i++) {
            float x = start_x + (i * (strum_width + strum_spacing));
            VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO, "about to create strum " + std::to_string(i));
            
            Strum* strum = new Strum(ctx, "strum.png", i);

            VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO, "strum " + std::to_string(i) + " created successfully");

            strum->setPosition(x, strum_y);
            strum->setSize(strum_width, strum_width);
            
            strums.push_back(strum);
        }

        properties.visible = true;
    }

    Playfield::~Playfield() {
        for (auto* strum : strums) {
            delete strum;
        }
        strums.clear();
    }

    void Playfield::update(float delta_time) {
        for (auto* strum : strums) {
            strum->update(delta_time);
        }
    }

    void Playfield::render() {
        if (!properties.visible) {
            return;
        }

        SolidComponent::render();

        for (auto* strum : strums) {
            strum->render();
        }
    }

    glm::vec2 Playfield::getDimensions() const {
        if (strums.empty()) {
            return glm::vec2(0.0f);
        }

        float total_width = 0.0f;
        float max_height = 0.0f;

        for (const auto* strum : strums) {
            total_width += strum->getSize().x;
            max_height = std::max(max_height, strum->getSize().y);
        }

        return glm::vec2(total_width, max_height);
    }
}