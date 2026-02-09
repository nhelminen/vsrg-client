#pragma once

#include <string>

#include "rhythm/charts/chart.hpp"


namespace mania {
class ManiaLoader : public IChartLoader {
public:
    ManiaLoader() = default;
    ~ManiaLoader() override = default;

    bool loadChart(const std::string& filepath, ChartData& out_data) override;

    bool canLoad(const std::string& filepath) const override { return filepath.ends_with(".osu"); }

    std::string getLoaderName() const override { return "osu!mania"; }

private:
    void parseGeneralSection(const std::string& line, ChartData& data);
    void parseMetadataSection(const std::string& line, ChartData& data);
    void parseDifficultySection(const std::string& line, ChartData& data);
    void parseTimingPointsSection(const std::string& line, ChartData& data);
    void parseEventsSection(const std::string& line, ChartData& data);
    void parseHitObjectsSection(const std::string& line, ChartData& data);

    int calculateColumn(int x, int key_count) const;
    std::string trim(const std::string& str) const;
};
}  // namespace mania