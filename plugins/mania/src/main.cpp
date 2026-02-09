#include "core/debug.hpp"
#include "core/ui/sprite.hpp"
#include "core/ui/spriteComponent.hpp"
#include "core/utils.hpp"
#include "public/IGamePlugin.hpp"
#include "public/engineContext.hpp"
#include "rhythm/charts/chart.hpp"
#include "rhythm/charts/mania.hpp"
#include "rhythm/playfield.hpp"

namespace mania {
class ManiaPlugin : public vsrg::IGamePlugin {
private:
    vsrg::EngineContext *ctx;

    ChartManager *chart_manager;
    vsrg::Conductor *conductor;

    vsrg::SpriteComponent *background;
    std::vector<Playfield *> playfields;

public:
    void init(vsrg::EngineContext *ctx) override {
        this->ctx = ctx;
        this->chart_manager = nullptr;
        this->conductor = nullptr;
        this->background = nullptr;

        ChartLoaderFactory::getInstance().registerLoader(std::make_shared<ManiaLoader>());
    }

    void load() override {
        VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO, "mania plugin loaded");

        chart_manager = new ChartManager(ctx);

        // hardcoded path for now lmfao
        std::string song_path = "charts/Rahatt - Matusa Bomber";
        std::string chart_path =
            vsrg::joinPaths(song_path, "Rahatt - Matusa Bomber (Ska) [2mnd].osu");
        if (!chart_manager->loadChart(chart_path)) {
            VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::ERROR,
                     "Failed to load chart from: " + chart_path);
            return;
        }

        conductor = chart_manager->createConductor();
        if (!conductor) {
            VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::ERROR, "Failed to create conductor");
            return;
        }

        // for debug, do playback speed here?
        conductor->set_playback_rate(1.5f);

        const ChartData *chart_data = chart_manager->getChartData();
        int key_count = chart_data->metadata.key_count;

        VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO,
                 "Successfully loaded: " + chart_data->metadata.title + " - " +
                     chart_data->metadata.artist + " [" + chart_data->metadata.difficulty + "]");

        Playfield *playfield = new Playfield(ctx, chart_data, conductor, key_count);
        playfields.push_back(playfield);

        // get the background sprite if it exists
        if (chart_data->metadata.background_file != "") {
            VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO,
                     "loading background at " + std::to_string(ctx->get_screen_width()) + "x" +
                         std::to_string(ctx->get_screen_height()));

            std::string backgroundPath =
                vsrg::joinPaths(song_path, chart_data->metadata.background_file);
            background = new vsrg::SpriteComponent(ctx, backgroundPath);

            vsrg::ComponentProperties sprite_properties = {
                true, 0.5f, 0.0f, 0, {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}};

            background->setProperties(sprite_properties);
            background->setSize({ctx->get_screen_width(), ctx->get_screen_height()});
            background->setLayer(0);  // Background at layer 0 (back)
        }
        conductor->play();
    }

    void update(float delta_time) override {
        if (conductor) {
            conductor->update(delta_time);
        }

        for (auto *playfield : playfields) {
            playfield->update(delta_time);
        }
    }

    void render() override {
        ctx->get_sprite_renderer()->begin();

        if (background != nullptr) background->render();

        ctx->get_sprite_renderer()->end();

        for (auto *playfield : playfields) {
            playfield->render();
        }
    }

    void unload() override {
        if (background) {
            delete background;
            background = nullptr;
        }

        for (auto *playfield : playfields) {
            delete playfield;
        }
        playfields.clear();

        if (conductor) {
            delete conductor;
            conductor = nullptr;
        }

        if (chart_manager) {
            delete chart_manager;
            chart_manager = nullptr;
        }
    }

    void shutdown() override {
        // when the entire plugin is destroyed
    }

    vsrg::PluginInfo get_info() const override {
        vsrg::PluginInfo info;
        info.name = "mania";
        info.version = "1.0.0";
        info.author = "ur mom";
        info.description = "osu!mania gameplay";
        return info;
    }
};
}  // namespace mania

PLUGIN_EXPORT vsrg::IGamePlugin *create_plugin() {
    return new mania::ManiaPlugin();
}

PLUGIN_EXPORT void destroy_plugin(vsrg::IGamePlugin *plugin) {
    delete plugin;
}