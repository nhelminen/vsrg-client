#include "rhythm/math/scroll.hpp"

namespace mania {
ScrollSpeedCalculator::ScrollSpeedCalculator(vsrg::Conductor* conductor)
    : mode(ScrollSpeedMode::XMOD), value(1.0f), conductor(conductor) {}

void ScrollSpeedCalculator::setXMod(float multiplier) {
    mode = ScrollSpeedMode::XMOD;
    value = multiplier;
}

void ScrollSpeedCalculator::setCMod(float constant_speed) {
    mode = ScrollSpeedMode::CMOD;
    value = constant_speed;
}

float ScrollSpeedCalculator::calculateScrollSpeed(float current_bpm) const {
    switch (mode) {
        case ScrollSpeedMode::XMOD:
            return current_bpm * value * (64.0f / 60.0f);

        case ScrollSpeedMode::CMOD:
            return (value / 60.0f) * 64.0f;

        default:
            return current_bpm * (64.0f / 60.0f);
    }
}

void ScrollSpeedCalculator::convertToCMod(float reference_bpm) {
    if (mode == ScrollSpeedMode::XMOD) {
        float new_value = value * reference_bpm;
        setCMod(new_value);
    }
}

void ScrollSpeedCalculator::convertToXMod(float reference_bpm) {
    if (mode == ScrollSpeedMode::CMOD && reference_bpm > 0.0f) {
        float new_value = value / reference_bpm;
        setXMod(new_value);
    }
}

float ScrollSpeedCalculator::getDisplaySpeed(float current_bpm) const {
    return calculateScrollSpeed(current_bpm);
}

float ScrollSpeedCalculator::getBPMAtTime(float time) const {
    if (!conductor) {
        return 120.0f;  // fallback
    }

    return conductor->get_bpm_at_time(time);
}

float ScrollSpeedCalculator::calculateDistanceThroughSections(float from_time,
                                                              float to_time) const {
    // stepmania formula:

    // CMOD (m_fTimeSpacing = 1.0):
    //   fBPS = fBPM / 60.0  (beats per second)
    //   fYOffset = fSecondsUntilStep * fBPS
    //   fYOffset *= ARROW_SPACING (64)
    // pixels_per_second = (cmod_value / 60.0) * 64.0

    // XMOD (m_fTimeSpacing = 0.0):
    //   fYOffset = beat_difference
    //   fYOffset *= ARROW_SPACING (64)
    //   fYOffset *= fScrollSpeed (the xmod multiplier)
    // pixels_per_second = BPM * multiplier * (64.0 / 60.0)

    // however, it doesnt match for some reason....
    // theres probably something done in the playfield, but i cannot be bothered to look for it rn

    if (!conductor) {
        float time_diff = to_time - from_time;

        if (mode == ScrollSpeedMode::CMOD) {
            float pixels_per_second = (value / 60.0f) * 64.0f;
            return time_diff * pixels_per_second;
        } else {
            float bpm = 120.0f;
            float pixels_per_second = bpm * value * (64.0f / 60.0f);
            return time_diff * pixels_per_second;
        }
    }

    const auto& all_points = conductor->get_timing_points();

    float total_distance = 0.0f;
    float section_start = from_time;

    for (const auto& point : all_points) {
        if (point.time > from_time && point.time <= to_time) {
            float section_end = point.time;
            float time_diff = section_end - section_start;
            float section_bpm = getBPMAtTime(section_start);

            float pixels_per_second;
            if (mode == ScrollSpeedMode::CMOD) {
                pixels_per_second = (value / 60.0f) * 64.0f;
            } else {
                pixels_per_second = section_bpm * value * (64.0f / 60.0f);
            }

            total_distance += time_diff * pixels_per_second;
            section_start = section_end;
        }
    }

    if (section_start < to_time) {
        float time_diff = to_time - section_start;
        float section_bpm = getBPMAtTime(section_start);

        float pixels_per_second;
        if (mode == ScrollSpeedMode::CMOD) {
            pixels_per_second = (value / 60.0f) * 64.0f;
        } else {
            pixels_per_second = section_bpm * value * (64.0f / 60.0f);
        }

        total_distance += time_diff * pixels_per_second;
    }

    return total_distance;
}

float ScrollSpeedCalculator::calculateNoteYPosition(float note_time, float current_time,
                                                    float strum_line_y) const {
    float time_diff = note_time - current_time;

    if (mode == ScrollSpeedMode::CMOD) {
        float pixels_per_second = (value / 60.0f) * 64.0f;
        float distance = time_diff * pixels_per_second;
        return strum_line_y - distance;
    }

    if (!conductor) {
        float bpm = 120.0f;
        float pixels_per_second = bpm * value * (64.0f / 60.0f);
        float distance = time_diff * pixels_per_second;
        return strum_line_y - distance;
    }

    float distance;
    if (note_time >= current_time) {
        distance = calculateDistanceThroughSections(current_time, note_time);
    } else {
        distance = -calculateDistanceThroughSections(note_time, current_time);
    }

    return strum_line_y - distance;
}
}  // namespace mania