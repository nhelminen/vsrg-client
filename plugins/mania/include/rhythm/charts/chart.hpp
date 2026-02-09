#pragma once

#include <memory>
#include <string>

#include "rhythm/charts/chartData.hpp"


namespace mania {
class IChartLoader {
public:
    virtual ~IChartLoader() = default;

    virtual bool loadChart(const std::string& filepath, ChartData& out_data) = 0;
    virtual bool canLoad(const std::string& filepath) const = 0;

    virtual std::string getLoaderName() const = 0;
};

class ChartLoaderFactory {
public:
    static ChartLoaderFactory& getInstance() {
        static ChartLoaderFactory instance;
        return instance;
    }

    void registerLoader(std::shared_ptr<IChartLoader> loader) { loaders.push_back(loader); }

    bool loadChart(const std::string& filepath, ChartData& out_data) {
        for (auto& loader : loaders) {
            if (loader->canLoad(filepath)) {
                return loader->loadChart(filepath, out_data);
            }
        }
        return false;
    }

    const std::vector<std::shared_ptr<IChartLoader>>& getLoaders() const { return loaders; }

private:
    ChartLoaderFactory() = default;
    std::vector<std::shared_ptr<IChartLoader>> loaders;
};

class ChartManager {
public:
    ChartManager(vsrg::EngineContext* ctx);
    ~ChartManager();

    bool loadChart(const std::string& filepath);

    const ChartData* getChartData() const { return current_chart.get(); }
    ChartData* getChartData() { return current_chart.get(); }

    vsrg::Conductor* createConductor();

    vsrg::Audio* getAudio() const { return audio; }
    bool hasChart() const { return current_chart != nullptr; }

private:
    vsrg::EngineContext* engine_context;

    std::unique_ptr<ChartData> current_chart;
    vsrg::Audio* audio;

    bool loadAudio(const std::string& audio_path);
};
}  // namespace mania