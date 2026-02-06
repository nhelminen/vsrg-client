#include "rhythm/note.hpp"

namespace mania {
    Note::Note(vsrg::EngineContext* ctx, const std::string& spritePath, int column, float time, NoteType type)
        : Strum(ctx, spritePath, column), time(time), type(type), can_render(false), speed_mod(1.0f) {
        
        properties.render_size = glm::vec2(64.0f, 64.0f);
    }

    Note::~Note() {
    }

    float Note::getTime() const {
        return time;
    }

    NoteType Note::getType() const {
        return type;
    }

    bool Note::canRender() const {
        return can_render;
    }

    void Note::setCanRender(bool render) {
        can_render = render;
    }

    float Note::getSpeedMod() const {
        return speed_mod;
    }

    void Note::setSpeedMod(float mod) {
        speed_mod = mod;
    }

    void Note::update(float deltaTime) {

    }

    void Note::render() {
        if (!can_render || isPressed() || !properties.visible) {
            return;
        }

        SpriteComponent::render();
    }
}