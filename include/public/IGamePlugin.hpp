#pragma once

#include <string>
#include <vector>

namespace vsrg
{
    class EngineContext;
    struct ChartData;
    struct InputEvent;

    struct PluginInfo
    {
        std::string name;
        std::string version;
        std::string author;
        std::vector<std::string> supported_formats;
        std::string description;
    };

    class IGamePlugin
    {
    public:
        virtual ~IGamePlugin() = default;

        virtual void init(EngineContext* ctx) = 0;
        virtual void load() = 0;
        virtual void update(float delta_time) = 0;
        virtual void render() = 0;
        virtual void unload() = 0;
        virtual void shutdown() = 0;

        virtual PluginInfo get_info() const = 0;
    };
}

#ifdef _WIN32
    #define PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
    #define PLUGIN_EXPORT extern "C"
#endif