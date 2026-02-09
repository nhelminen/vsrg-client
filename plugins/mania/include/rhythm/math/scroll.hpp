#pragma once

#include <algorithm>
#include <cmath>

#include "rhythm/conductor.hpp"


namespace mania {
enum class ScrollSpeedMode {
    XMOD,  // multiplier of bpm (classic)
    CMOD,  // constant scroll speed
};

class ScrollSpeedCalculator {
public:
    ScrollSpeedCalculator(vsrg::Conductor* conductor = nullptr);
    ~ScrollSpeedCalculator() = default;

    void setXMod(float multiplier);
    void setCMod(float constant_speed);

    ScrollSpeedMode getMode() const { return mode; }
    float getValue() const { return value; }

    void setConductor(vsrg::Conductor* cond) { conductor = cond; }
    float calculateScrollSpeed(float current_bpm) const;

    void convertToCMod(float reference_bpm);
    void convertToXMod(float reference_bpm);

    float getDisplaySpeed(float current_bpm) const;
    float calculateNoteYPosition(float note_time, float current_time, float strum_line_y) const;

private:
    ScrollSpeedMode mode;
    float value;
    vsrg::Conductor* conductor;

    float getBPMAtTime(float time) const;

    float calculateDistanceThroughSections(float from_time, float to_time) const;
};
}  // namespace mania