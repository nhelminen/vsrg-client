#include "public/IGamePlugin.hpp"
#include "public/engineContext.hpp"
#include "core/debug.hpp"

class ManiaPlugin : public vsrg::IGamePlugin
{
private:
    vsrg::EngineContext* ctx;

public:
    void init(vsrg::EngineContext* ctx) override {
        this->ctx = ctx;
    }

    void load() override {
        // called hwne loading into gameplay
        VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO, "mania plugin loaded");
    }

    void update(float delta_time) override {
        // update stuff
    }

    void render() override {
        // rendering
    }

    void unload() override {
        // cleanup from leaving gmaeplay
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

PLUGIN_EXPORT vsrg::IGamePlugin* create_plugin() {
    return new ManiaPlugin();
}

PLUGIN_EXPORT void destroy_plugin(vsrg::IGamePlugin* plugin) {
    delete plugin;
}