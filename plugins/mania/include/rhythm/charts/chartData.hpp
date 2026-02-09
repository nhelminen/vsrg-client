#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "rhythm/conductor.hpp"


namespace mania {
enum class VSRGNoteType { TAP, HOLD, MINE, ROLL };

struct VSRGNote {
    int column;
    float time;
    float end_time;
    VSRGNoteType type;

    VSRGNote(int col, float t, VSRGNoteType nt = VSRGNoteType::TAP)
        : column(col), time(t), end_time(t), type(nt) {}

    VSRGNote(int col, float t, float et, VSRGNoteType nt)
        : column(col), time(t), end_time(et), type(nt) {}
};

struct ChartMetadata {
    std::string title;
    std::string subtitle;
    std::string artist;
    std::string charter;
    std::string difficulty;
    std::string audio_file;
    std::string background_file;

    int key_count;
    float preview_time;
    float offset;

    ChartMetadata() : key_count(4), preview_time(0.0f), offset(0.0f) {}
};

struct ChartData {
    ChartMetadata metadata;
    std::vector<VSRGNote> notes;
    std::vector<vsrg::TimingPoint> timing_points;

    std::vector<VSRGNote> getNotesForColumn(int column) const {
        std::vector<VSRGNote> result;
        for (const auto& note : notes) {
            if (note.column == column) {
                result.push_back(note);
            }
        }
        return result;
    }

    void sortNotes() {
        std::sort(notes.begin(), notes.end(),
                  [](const VSRGNote& a, const VSRGNote& b) { return a.time < b.time; });
    }

    void sortTimingPoints() {
        std::sort(
            timing_points.begin(), timing_points.end(),
            [](const vsrg::TimingPoint& a, const vsrg::TimingPoint& b) { return a.time < b.time; });
    }
};
}  // namespace mania