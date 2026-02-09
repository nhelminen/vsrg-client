#pragma once

#include <memory>
#include <string>
#include <vector>

#include "public/IGamePlugin.hpp"


namespace vsrg {
class EngineContext;
class Debugger;

class PluginManager {
public:
    PluginManager(EngineContext* engine_context);
    ~PluginManager();

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    void discover_plugins(const std::string& plugins_dir);
    bool load_plugin(const std::string& plugin_path);
    void unload_plugin(const std::string& plugin_name);
    void unload_all_plugins();

    std::vector<PluginInfo> get_available_plugins() const;

    bool activate_plugin(const std::string& plugin_name);
    void deactivate_current_plugin();

    IGamePlugin* get_active_plugin() const { return current_plugin; }
    bool has_active_plugin() const { return current_plugin != nullptr; }

    IGamePlugin* find_plugin(const std::string& plugin_name);

private:
    struct LoadedPlugin {
        void* dll_handle;
        IGamePlugin* instance;
        PluginInfo info;
        std::string file_path;
        bool is_active;

        LoadedPlugin() : dll_handle(nullptr), instance(nullptr), is_active(false) {}
    };

    EngineContext* engine_context;

    std::vector<LoadedPlugin> loaded_plugins;
    IGamePlugin* current_plugin;

    void* load_dll(const std::string& path);
    void unload_dll(void* handle);
    void* get_dll_function(void* handle, const std::string& function_name);

    LoadedPlugin* find_plugin_by_name(const std::string& name);
};
}  // namespace vsrg