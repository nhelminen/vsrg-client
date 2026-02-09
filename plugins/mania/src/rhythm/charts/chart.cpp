#include "rhythm/charts/chart.hpp"

#include <filesystem>

#include "core/debug.hpp"
#include "core/utils.hpp"


namespace mania {
ChartManager::ChartManager(vsrg::EngineContext* ctx) : engine_context(ctx), audio(nullptr) {}

ChartManager::~ChartManager() {}

bool ChartManager::loadChart(const std::string& path) {
    std::string execPath = vsrg::getExecutableDir();
    std::string filePath = vsrg::joinPaths(execPath, "assets", path);

    VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::INFO,
             "Loading chart from " + filePath);

    auto new_chart = std::make_unique<ChartData>();
    if (!ChartLoaderFactory::getInstance().loadChart(filePath, *new_chart)) {
        VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::ERROR, "Failed to load chart");
        return false;
    }

    VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::INFO,
             "Chart loaded!! " + new_chart->metadata.title + " - " + new_chart->metadata.artist +
                 " [" + new_chart->metadata.difficulty + "]");

    std::filesystem::path chart_path(filePath);
    std::filesystem::path chart_dir = chart_path.parent_path();
    std::filesystem::path audio_path = chart_dir / new_chart->metadata.audio_file;

    if (!loadAudio(audio_path.string())) {
        VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::WARNING,
                 "Failed to load audio, but chart is still loaded");
    }

    current_chart = std::move(new_chart);
    return true;
}

vsrg::Conductor* ChartManager::createConductor() {
    if (!current_chart) {
        VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::ERROR,
                 "Cannot create conductor without a loaded chart");
        return nullptr;
    }

    if (!audio) {
        VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::WARNING,
                 "Creating conductor without audio");
    }

    return new vsrg::Conductor(engine_context->get_audio_manager(), audio,
                               current_chart->timing_points);
}

bool ChartManager::loadAudio(const std::string& audio_path) {
    VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::INFO,
             "Loading audio from " + audio_path);

    vsrg::AudioResult audioResult = engine_context->get_audio_manager()->load_audio(audio_path);

    if (audioResult.status != MA_SUCCESS) {
        VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::ERROR, "Failed to load audio");
        audio = nullptr;
        return false;
    }

    audio = audioResult.audio;
    VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::INFO,
             "Audio loaded successfully!!");
    return true;
}
}  // namespace mania