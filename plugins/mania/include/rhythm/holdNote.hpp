#pragma once

#include <memory>

#include "core/ui/spriteComponent.hpp"
#include "rhythm/note.hpp"

namespace mania {
class HoldNote : public Note {
public:
    HoldNote(vsrg::EngineContext *ctx, const std::string &spritePath, int column, float time,
             float endTime);
    virtual ~HoldNote();

    float getEndTime() const { return end_time; }
    float getDuration() const;

    bool isHolding() const { return is_holding; }
    void setHolding(bool holding) { is_holding = holding; }

    void setEndY(float y) { end_y = y; }
    float getEndY() const { return end_y; }

    void setHoldEndHeight(float height) { hold_end_height = height; }
    float getHoldEndHeight() const { return hold_end_height; }

    void startFadeOut() { is_fading_out = true; }
    bool isFadingOut() const { return is_fading_out; }

    bool shouldDespawn() const override;

    void update(float deltaTime) override;
    void render() override;

private:
    float end_time;
    bool is_holding = false;
    bool is_fading_out = false;

    float end_y = 0.0f;
    float hold_end_height = -1.0f;

    std::string hold_body_path;
    std::string hold_end_path;

    std::unique_ptr<vsrg::SpriteComponent> hold_body_sprite;
    std::unique_ptr<vsrg::SpriteComponent> hold_end_sprite;
};
}  // namespace mania