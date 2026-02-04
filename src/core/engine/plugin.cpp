#include "core/engine/plugin.hpp"
#include "public/engineContext.hpp"
#include "core/debug.hpp"

#include <filesystem>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace vsrg
{
    PluginManager::PluginManager(EngineContext* engine_context)
        : engine_context(engine_context), current_plugin(nullptr)
    {

    }

    PluginManager::~PluginManager()
    {
        unload_all_plugins();
    }

    void* PluginManager::load_dll(const std::string& path)
    {
#ifdef _WIN32
        return (void*)LoadLibraryA(path.c_str());
#else
        return dlopen(path.c_str(), RTLD_NOW);
#endif
    }

    void PluginManager::unload_dll(void* handle)
    {
        if (!handle) return;

#ifdef _WIN32
        FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
    }

    void* PluginManager::get_dll_function(void* handle, const std::string& function_name)
    {
        if (!handle) return nullptr;

#ifdef _WIN32
        return (void*)GetProcAddress((HMODULE)handle, function_name.c_str());
#else
	dlerror(); // clear old errors
        void* sym = dlsym(handle, function_name.c_str());
	const char* error = dlerror();
	if (error)
	{
		return nullptr;
	}
	return sym;
#endif
    }

    void PluginManager::discover_plugins(const std::string& plugins_dir)
    {
        namespace fs = std::filesystem;

        if (!fs::exists(plugins_dir) || !fs::is_directory(plugins_dir))
        {
            return;
        }

        int plugins_found = 0;

#ifdef _WIN32
        std::string dll_extension = ".dll";
#else
        std::string dll_extension = ".so";
#endif

        for (const auto& entry : fs::directory_iterator(plugins_dir))
        {
            if (entry.is_regular_file())
            {
                std::string file_path = entry.path().string();
                std::string extension = entry.path().extension().string();

                if (extension == dll_extension)
                {
                    if (load_plugin(file_path))
                    {
                        plugins_found++;
                    }
                }
            }
        }
    }

    bool PluginManager::load_plugin(const std::string& plugin_path)
    {
        void* dll_handle = load_dll(plugin_path);
        if (!dll_handle)
        {
            return false;
        }

        typedef IGamePlugin* (*CreatePluginFunc)();
        CreatePluginFunc create_plugin = (CreatePluginFunc)get_dll_function(dll_handle, "create_plugin");

        if (!create_plugin)
        {
            unload_dll(dll_handle);
            return false;
        }

        IGamePlugin* plugin_instance = create_plugin();
        if (!plugin_instance)
        {
            unload_dll(dll_handle);
            return false;
        }

        try
        {
            plugin_instance->init(engine_context);
        }
        catch (const std::exception& e)
        {
            delete plugin_instance;
            unload_dll(dll_handle);
            return false;
        }

        PluginInfo info = plugin_instance->get_info();

        if (find_plugin_by_name(info.name))
        {
            plugin_instance->shutdown();
            delete plugin_instance;
            unload_dll(dll_handle);
            return false;
        }

        LoadedPlugin loaded;
        loaded.dll_handle = dll_handle;
        loaded.instance = plugin_instance;
        loaded.info = info;
        loaded.file_path = plugin_path;
        loaded.is_active = false;

        loaded_plugins.push_back(loaded);

        return true;
    }

    void PluginManager::unload_plugin(const std::string& plugin_name)
    {
        auto it = std::find_if(loaded_plugins.begin(), loaded_plugins.end(),
            [&plugin_name](const LoadedPlugin& p) {
                return p.info.name == plugin_name;
            });

        if (it == loaded_plugins.end())
        {
            return;
        }

        if (it->is_active)
        {
            deactivate_current_plugin();
        }

        if (it->instance)
        {
            it->instance->shutdown();
            
            typedef void (*DestroyPluginFunc)(IGamePlugin*);
            DestroyPluginFunc destroy_plugin = 
                (DestroyPluginFunc)get_dll_function(it->dll_handle, "destroy_plugin");
            
            if (destroy_plugin)
            {
                destroy_plugin(it->instance);
            }
        }

        unload_dll(it->dll_handle);
        loaded_plugins.erase(it);
    }

    void PluginManager::unload_all_plugins()
    {
        deactivate_current_plugin();

        while (!loaded_plugins.empty())
        {
            unload_plugin(loaded_plugins[0].info.name);
        }
    }

    std::vector<PluginInfo> PluginManager::get_available_plugins() const
    {
        std::vector<PluginInfo> plugins;
        plugins.reserve(loaded_plugins.size());

        for (const auto& plugin : loaded_plugins)
        {
            plugins.push_back(plugin.info);
        }

        return plugins;
    }

    IGamePlugin* PluginManager::find_plugin(const std::string& plugin_name)
    {
        LoadedPlugin* plugin = find_plugin_by_name(plugin_name);
        if (!plugin)
        {
            return nullptr;
        }

        return plugin->instance;
    }

    bool PluginManager::activate_plugin(const std::string& plugin_name)
    {
        if (current_plugin)
        {
            deactivate_current_plugin();
        }

        LoadedPlugin* plugin = find_plugin_by_name(plugin_name);
        if (!plugin)
        {
            return false;
        }

        plugin->is_active = true;
        current_plugin = plugin->instance;

        return true;
    }

    void PluginManager::deactivate_current_plugin()
    {
        if (!current_plugin)
            return;

        for (auto& plugin : loaded_plugins)
        {
            if (plugin.instance == current_plugin)
            {
                plugin.is_active = false;
                break;
            }
        }

        current_plugin = nullptr;
    }

    PluginManager::LoadedPlugin* PluginManager::find_plugin_by_name(const std::string& name)
    {
        for (auto& plugin : loaded_plugins)
        {
            if (plugin.info.name == name)
            {
                return &plugin;
            }
        }
        return nullptr;
    }
}
