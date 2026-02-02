#pragma once

namespace vsrg
{
    class Client;
    class Debugger;
	class AudioManager;
	class ScreenManager;

    // this is a safe interface to expose to screens or any future plugins
    class EngineContext
    {
    public:
        EngineContext(Client* client);
		~EngineContext() = default;

        // prevent copying/moving !!!
        EngineContext(const EngineContext&) = delete;
		EngineContext& operator=(const EngineContext&) = delete;
		EngineContext(EngineContext&&) = delete;
		EngineContext& operator=(EngineContext&&) = delete;

        // single update call to fetch classes
        void update();
        
        // base getters for the classes themselves
        Debugger* get_debugger() const { return debugger; }
        AudioManager* get_audio_manager() const { return audio_manager; }
        ScreenManager* get_screen_manager() const { return screen_manager; }

        // convenience getters for common data (define in cpp)
        int get_screen_width() const;
        int get_screen_height() const;
        float get_delta_time() const;

        // add anything that might be commonly needed here later btw
    private:
        Client* client; // keep reference

        Debugger* debugger;
        AudioManager* audio_manager;
        ScreenManager* screen_manager;
    };
}