#include "rhythm/holdNote.hpp"

namespace mania {
HoldNote::HoldNote(vsrg::EngineContext *ctx, const std::string &spritePath, int column, float time,
                   float endTime)
    : Note(ctx, spritePath, column, time, NoteType::HOLD),
      end_time(endTime),
      is_holding(false),
      is_fading_out(false) {
    hold_body_path = "holdBody" + std::to_string(column) + ".png";
    hold_end_path = "holdEnd" + std::to_string(column) + ".png";

    hold_body_sprite = std::make_unique<vsrg::SpriteComponent>(ctx, hold_body_path);
    hold_body_sprite->setRenderMode(vsrg::RenderMode::Stretch);

    hold_end_sprite = std::make_unique<vsrg::SpriteComponent>(ctx, hold_end_path);
    hold_end_sprite->setRenderMode(vsrg::RenderMode::Stretch);
}

HoldNote::~HoldNote() {}

float HoldNote::getDuration() const {
    return end_time - getTime();
}

bool HoldNote::shouldDespawn() const {
    if (is_fading_out) {
        float note_height = properties.render_size.y;
        float hold_end_y = end_y + note_height;
        float screen_height = static_cast<float>(engine_context->get_screen_height());

        if (hold_end_y > screen_height) {
            return true;
        }
        return false;
    }

    return Note::shouldDespawn();
}

void HoldNote::update(float deltaTime) {
    Note::update(deltaTime);

    if (is_holding) {
        return;
    }
}

void HoldNote::render() {
    if (!canRender() || !properties.visible) {
        return;
    }

    if (shouldDespawn()) {
        return;
    }

    if (!is_holding && isPressed() && !is_fading_out) {
        return;
    }

    auto *body_texture = engine_context->get_texture_cache()->getTexture(hold_body_path);
    auto *end_texture = engine_context->get_texture_cache()->getTexture(hold_end_path);

    float note_x = properties.position.x;
    float note_y = properties.position.y;
    float note_width = properties.render_size.x;
    float note_height = properties.render_size.y;

    if (hold_end_height < 0.0f && end_texture && end_texture->loaded) {
        hold_end_height = static_cast<float>(end_texture->dimensions.y);
    }

    float body_start_y = note_y + (note_height / 2.0f);
    float body_end_y = end_y;

    float total_body_height = std::abs(body_end_y - body_start_y);
    float actual_end_height = std::min(hold_end_height, total_body_height);

    float body_texture_height = total_body_height - actual_end_height;
    float alpha = is_fading_out ? 0.5f : 1.0f;

    if (total_body_height > 1.0f && end_time > 0.0f) {
        if (end_texture && end_texture->loaded && actual_end_height > 0.0f && hold_end_sprite) {
            hold_end_sprite->setPosition(glm::vec2(note_x, end_y));
            hold_end_sprite->setSize(glm::vec2(note_width, actual_end_height));

            float full_end_height = hold_end_height;
            float v_height_end = 1.0f;
            if (full_end_height > 0.0f) {
                v_height_end = actual_end_height / full_end_height;
                if (v_height_end > 1.0f) v_height_end = 1.0f;
            }
            hold_end_sprite->setUVRect(glm::vec4(0.0f, 0.0f, 1.0f, v_height_end));

            hold_end_sprite->setRotation(properties.rotation);
            hold_end_sprite->setAnchor(properties.anchor);
            hold_end_sprite->setScale(properties.scale);
            hold_end_sprite->setOpacity(alpha * properties.opacity);
            hold_end_sprite->setLayer(properties.layer - 0.1);
            hold_end_sprite->setVisible(true);

            hold_end_sprite->render();
        }

        if (body_texture && body_texture->loaded && body_texture_height > 0.0f &&
            hold_body_sprite) {
            hold_body_sprite->setPosition(glm::vec2(note_x, end_y + actual_end_height));
            hold_body_sprite->setSize(glm::vec2(note_width, body_texture_height));

            float full_body_height = body_texture ? (float)body_texture->dimensions.y : 0.0f;
            float original_body_height = full_body_height;

            if (full_body_height <= 0.0f) full_body_height = body_texture_height;

            float v_height = 1.0f;
            if (full_body_height > 0.0f) {
                v_height = body_texture_height / full_body_height;
                if (v_height > 1.0f) v_height = 1.0f;
            }

            hold_body_sprite->setUVRect(glm::vec4(0.0f, 0.0f, 1.0f, v_height));

            hold_body_sprite->setRotation(properties.rotation);
            hold_body_sprite->setAnchor(properties.anchor);
            hold_body_sprite->setScale(properties.scale);
            hold_body_sprite->setOpacity(alpha * properties.opacity);
            hold_body_sprite->setLayer(properties.layer - 0.1);
            hold_body_sprite->setVisible(true);

            hold_body_sprite->render();
        }
    }

    if (!is_fading_out) {
        Note::render();
    }
}
}  // namespace mania