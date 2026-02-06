#include "rhythm/holdNote.hpp"

namespace mania {
    HoldNote::HoldNote(vsrg::EngineContext* ctx, const std::string& spritePath, int column, float time, float endTime)
        : Note(ctx, spritePath, column, time, NoteType::HOLD), end_time(endTime), is_holding(false) {
    }

    HoldNote::~HoldNote() {
    }

    float HoldNote::getEndTime() const {
        return end_time;
    }

    float HoldNote::getDuration() const {
        return end_time - getTime();
    }

    bool HoldNote::isHolding() const {
        return is_holding;
    }

    void HoldNote::setHolding(bool holding) {
        is_holding = holding;
    }

    void HoldNote::update(float deltaTime) {
        Note::update(deltaTime);
    }

    void HoldNote::render() {
        if (!canRender() || !properties.visible) {
            return;
        }

        if (!isPressed() || is_holding) {
            SpriteComponent::render();
        }
    }
}