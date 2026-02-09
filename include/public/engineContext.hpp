#pragma once

namespace vsrg {
class Client;
class Debugger;
class FontManager;
class AudioManager;
class TextureCache;
class ScreenManager;
class PluginManager;
class SpriteRenderer;

// this is a safe interface to expose to screens or any future plugins
class EngineContext {
public:
    EngineContext(Client* client);
    ~EngineContext();

    // prevent copying/moving !!!
    EngineContext(const EngineContext&) = delete;
    EngineContext& operator=(const EngineContext&) = delete;
    EngineContext(EngineContext&&) = delete;
    EngineContext& operator=(EngineContext&&) = delete;

    // base getters for the classes themselves
    Debugger* get_debugger() const { return debugger; }
    FontManager* get_font_manager() const { return font_manager; }
    AudioManager* get_audio_manager() const { return audio_manager; }
    TextureCache* get_texture_cache() const { return texture_cache; }
    ScreenManager* get_screen_manager() const { return screen_manager; }
    PluginManager* get_plugin_manager() const { return plugin_manager; }
    SpriteRenderer* get_sprite_renderer() const { return sprite_renderer; }

    // convenience getters for common data (define in cpp)
    int get_screen_width() const;
    int get_screen_height() const;
    float get_delta_time() const;

    // add anything that might be commonly needed here later btw
private:
    Client* client;  // keep reference

    Debugger* debugger;
    FontManager* font_manager;
    AudioManager* audio_manager;
    TextureCache* texture_cache;
    ScreenManager* screen_manager;
    PluginManager* plugin_manager;
    SpriteRenderer* sprite_renderer;
};
}  // namespace vsrg