#include "rhythm/strum.hpp"

#include "core/debug.hpp"

namespace mania {
Strum::Strum(vsrg::EngineContext *ctx, const std::string &spritePath, int column)
    : vsrg::SpriteComponent(ctx, spritePath), column(column), is_pressed(false) {
    properties.render_size = glm::vec2(64.0f, 64.0f);
    properties.position = glm::vec2(0.0f, 0.0f);
    properties.layer = 2;  // Strums at layer 2 (above playfield background)
}

Strum::~Strum() {}

void Strum::setPosition(float x, float y) {
    properties.position = glm::vec2(x, y);
}

void Strum::setSize(float width, float height) {
    properties.render_size = glm::vec2(width, height);
}

void Strum::setPadding(float top, float bottom, float left, float right) {
    properties.position.x += left;
    properties.position.y += top;
}

void Strum::update(float deltaTime) {
    if (is_pressed) {
        properties.opacity = 0.8f;
    } else {
        properties.opacity = 1.0f;
    }
}

void Strum::render() {
    if (!properties.visible) {
        return;
    }

    SpriteComponent::render();
}
}  // namespace mania