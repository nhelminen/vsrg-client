#include "rhythm/charts/mania.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>


namespace mania {
bool ManiaLoader::loadChart(const std::string& filepath, ChartData& out_data) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    out_data.notes.clear();
    out_data.timing_points.clear();

    std::string line;
    std::string current_section = "";

    while (std::getline(file, line)) {
        line = trim(line);

        if (line.empty() || line[0] == '/' || line.find("//") == 0) {
            continue;
        }

        if (line[0] == '[' && line[line.length() - 1] == ']') {
            current_section = line.substr(1, line.length() - 2);
            continue;
        }

        if (current_section == "General") {
            parseGeneralSection(line, out_data);
        } else if (current_section == "Metadata") {
            parseMetadataSection(line, out_data);
        } else if (current_section == "Difficulty") {
            parseDifficultySection(line, out_data);
        } else if (current_section == "TimingPoints") {
            parseTimingPointsSection(line, out_data);
        } else if (current_section == "Events") {
            parseEventsSection(line, out_data);
        } else if (current_section == "HitObjects") {
            parseHitObjectsSection(line, out_data);
        }
    }

    file.close();

    out_data.sortNotes();
    out_data.sortTimingPoints();

    return true;
}

void ManiaLoader::parseGeneralSection(const std::string& line, ChartData& data) {
    size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) return;

    std::string key = trim(line.substr(0, colon_pos));
    std::string value = trim(line.substr(colon_pos + 1));

    if (key == "AudioFilename") {
        data.metadata.audio_file = value;
    } else if (key == "PreviewTime") {
        data.metadata.preview_time = std::stof(value) / 1000.0f;
    } else if (key == "AudioLeadIn") {
        data.metadata.offset = std::stof(value) / 1000.0f;
    }
}

void ManiaLoader::parseMetadataSection(const std::string& line, ChartData& data) {
    size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) return;

    std::string key = trim(line.substr(0, colon_pos));
    std::string value = trim(line.substr(colon_pos + 1));

    if (key == "Title") {
        data.metadata.title = value;
    } else if (key == "Artist") {
        data.metadata.artist = value;
    } else if (key == "Creator") {
        data.metadata.charter = value;
    } else if (key == "Version") {
        data.metadata.difficulty = value;
    }
}

void ManiaLoader::parseEventsSection(const std::string& line, ChartData& data) {
    if (line.compare(0, 2, "0,") != 0) return;
    if (data.metadata.background_file != "") return;

    size_t firstQuote = line.find('"');
    size_t lastQuote = line.find('"', firstQuote + 1);

    if (firstQuote != std::string::npos && lastQuote != std::string::npos) {
        std::string filename = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
        data.metadata.background_file = filename;
    }
}

void ManiaLoader::parseDifficultySection(const std::string& line, ChartData& data) {
    size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) return;

    std::string key = trim(line.substr(0, colon_pos));
    std::string value = trim(line.substr(colon_pos + 1));

    if (key == "CircleSize") {
        data.metadata.key_count = std::stoi(value);
    }
}

void ManiaLoader::parseTimingPointsSection(const std::string& line, ChartData& data) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    if (tokens.size() < 2) return;

    float time = std::stof(tokens[0]) / 1000.0f;
    float beat_length = std::stof(tokens[1]);
    int meter = tokens.size() > 2 ? std::stoi(tokens[2]) : 4;
    bool uninherited = tokens.size() > 6 ? (std::stoi(tokens[6]) == 1) : true;

    if (uninherited && beat_length > 0) {
        vsrg::TimingPoint tp;
        tp.time = time;
        tp.bpm = 60000.0 / beat_length;
        tp.nominator = meter;
        tp.denominator = 4;

        data.timing_points.push_back(tp);
    }
}

void ManiaLoader::parseHitObjectsSection(const std::string& line, ChartData& data) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    if (tokens.size() < 4) return;

    int x = std::stoi(tokens[0]);
    float time = std::stof(tokens[2]) / 1000.0f;
    int type = std::stoi(tokens[3]);

    int column = calculateColumn(x, data.metadata.key_count);

    if (type & 128) {
        if (tokens.size() > 5) {
            size_t colon_pos = tokens[5].find(':');
            if (colon_pos != std::string::npos) {
                float end_time = std::stof(tokens[5].substr(0, colon_pos)) / 1000.0f;
                data.notes.emplace_back(column, time, end_time, VSRGNoteType::HOLD);
            }
        }
    } else {
        data.notes.emplace_back(column, time, VSRGNoteType::TAP);
    }
}

int ManiaLoader::calculateColumn(int x, int key_count) const {
    float column_width = 512.0f / key_count;
    int column = static_cast<int>(x / column_width);

    if (column < 0) column = 0;
    if (column >= key_count) column = key_count - 1;

    return column;
}

std::string ManiaLoader::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";

    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}
}  // namespace mania