#pragma once

#include "public/engineContext.hpp"
#include "core/ui/solidComponent.hpp"
#include "rhythm/strum.hpp"

#include <vector>

namespace mania {
    class Playfield : public vsrg::SolidComponent {
    public:
        Playfield(vsrg::EngineContext* ctx, int key_count, glm::vec4 background_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
        ~Playfield();

        void update(float delta_time);
        void render() override;

        glm::vec2 getDimensions() const override;
    private:
        vsrg::EngineContext* engine_context;

        int key_count;
        std::vector<Strum*> strums;
    };
}