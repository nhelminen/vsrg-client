#include "public/IGamePlugin.hpp"
#include "public/engineContext.hpp"
#include "core/debug.hpp"
#include "rhythm/playfield.hpp"

namespace mania {
class ManiaPlugin : public vsrg::IGamePlugin
    {
    private:
        vsrg::EngineContext* ctx;
        Playfield* playfield;

    public:
        void init(vsrg::EngineContext* ctx) override {
            this->ctx = ctx;
            this->playfield = nullptr;
        }

        void load() override {
            // called hwne loading into gameplay
            VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO, "mania plugin loaded");

            int key_count = 4;
            playfield = new Playfield(ctx, key_count);
            
            VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO, "playfield created with " + std::to_string(key_count) + " keys");
        }

        void update(float delta_time) override {
            // update stuff

            if (playfield) {
                playfield->update(delta_time);
            }
        }

        void render() override {
            // rendering

            if (playfield) {
                playfield->render();
            }
        }

        void unload() override {
            // cleanup from leaving gmaeplay

            if (playfield) {
                delete playfield;
                playfield = nullptr;
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
}

PLUGIN_EXPORT vsrg::IGamePlugin* create_plugin() {
    return new mania::ManiaPlugin();
}

PLUGIN_EXPORT void destroy_plugin(vsrg::IGamePlugin* plugin) {
    delete plugin;
}