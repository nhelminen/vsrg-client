#include "public/engineContext.hpp"

#include "core/app.hpp"
#include "core/debug.hpp"
#include "core/engine/audio.hpp"
#include "core/engine/plugin.hpp"
#include "core/engine/screen.hpp"
#include "core/ui/font.hpp"
#include "core/ui/sprite.hpp"
#include "core/ui/texture.hpp"


namespace vsrg {
EngineContext::EngineContext(Client* client) : client(client) {
    debugger = new Debugger();

    font_manager = new FontManager(this);
    audio_manager = new AudioManager(this);
    texture_cache = new TextureCache(this);

    screen_manager = new ScreenManager(this);
    plugin_manager = new PluginManager(this);

    sprite_renderer = new SpriteRenderer(this);
}

EngineContext::~EngineContext() {
    if (plugin_manager != nullptr) {
        delete plugin_manager;
        plugin_manager = nullptr;
    }
    if (font_manager != nullptr) {
        delete font_manager;
        font_manager = nullptr;
    }
    if (screen_manager != nullptr) {
        delete screen_manager;
        screen_manager = nullptr;
    }
    if (sprite_renderer != nullptr) {
        delete sprite_renderer;
        sprite_renderer = nullptr;
    }
    if (texture_cache != nullptr) {
        delete texture_cache;
        texture_cache = nullptr;
    }
    if (audio_manager != nullptr) {
        delete audio_manager;
        audio_manager = nullptr;
    }
    if (debugger != nullptr) {
        delete debugger;
        debugger = nullptr;
    }
}

int EngineContext::get_screen_width() const {
    return client->get_screen_width();
}

int EngineContext::get_screen_height() const {
    return client->get_screen_height();
}

float EngineContext::get_delta_time() const {
    return client->get_delta_time();
}
}  // namespace vsrg