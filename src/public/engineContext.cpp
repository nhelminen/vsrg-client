#include "public/engineContext.hpp"
#include "core/app.hpp"

namespace vsrg
{
    EngineContext::EngineContext(Client* client)
        : client(client)
    {
    }

    void EngineContext::update()
    {
        debugger = client->get_debugger();
        audio_manager = client->get_audio_manager();
        screen_manager = client->get_screen_manager();
        plugin_manager = client->get_plugin_manager();
    }

    int EngineContext::get_screen_width() const
    {
        return client->get_screen_width();
    }

    int EngineContext::get_screen_height() const
    {
        return client->get_screen_height();
    }

    float EngineContext::get_delta_time() const
    {
        return client->get_delta_time();
    }
}