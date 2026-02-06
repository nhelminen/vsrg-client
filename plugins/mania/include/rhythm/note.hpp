#pragma once

#include "rhythm/strum.hpp"

namespace mania {
    enum class NoteType {
        NORMAL,
        HOLD,
        MINE
        // add more here later if necessary
    };

    class Note : public Strum {
    public:
        Note(vsrg::EngineContext* ctx, const std::string& spritePath, int column, float time, NoteType type);
        virtual ~Note();

        float getTime() const;
        NoteType getType() const;

        bool canRender() const;
        void setCanRender(bool render);

        float getSpeedMod() const;
        void setSpeedMod(float mod);

        void update(float deltaTime) override;
        void render() override;
    private:
        float time;
        NoteType type;

        bool can_render = false;
        float speed_mod = 1.0f; // for future per-note sv bullshit
    };
}