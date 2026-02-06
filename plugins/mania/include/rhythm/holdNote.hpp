#pragma once

#include "rhythm/note.hpp"

namespace mania {
    class HoldNote : public Note {
    public:
        HoldNote(vsrg::EngineContext* ctx, const std::string& spritePath, int column, float time, float endTime);
        virtual ~HoldNote();

        float getEndTime() const;
        float getDuration() const;

        bool isHolding() const;
        void setHolding(bool holding);

        void update(float deltaTime) override;
        void render() override;
    private:
        float end_time;
        bool is_holding = false;
    };
}