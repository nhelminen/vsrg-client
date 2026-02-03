#include "core/screens/debugScreen.hpp"
#include "core/engine/shader.hpp"
#include "core/engine/audio.hpp"
#include "core/debug.hpp"
#include "core/utils.hpp"

#include "public/engineContext.hpp"

namespace vsrg
{
    DebugScreen::DebugScreen(EngineContext* engine_context)
        : Screen(engine_context, "DebugScreen", 1),
        font_manager(engine_context),
        font(engine_context, font_manager.getFt(), "assets/fonts/NotoSansJP-Regular.ttf", 32),
        text_component(engine_context, &font),
        sprite_component(engine_context, "matusa.png")
    {
        ComponentProperties properties = {
            true,
            0.5f, -4.0f,
            { 16.0f, 16.0f },
            { 1.2f, 0.9f },
            { 0.0f, 0.0f },
        };
        TextRenderOptions text_options = { 
            { 1.0f, 1.0f, 1.0f }, 
            12.0f, 
            4.0f 
        };

        text_component.setText("?");
        text_component.setProperties(properties);
        text_component.setTextOptions(text_options);

        ComponentProperties sprite_properties = {
            true,
            1.0f, 0.0f,
            { 16.0f, 48.0f },
            { 2.0f, 2.0f },
            { 0.0f, 0.0f },
        };
        
        sprite_component.setProperties(sprite_properties);

        Debugger* debugger = engine_context->get_debugger();
        debugger->log(DebugLevel::INFO, "DebugScreen loaded", __FILE__, __LINE__);

        AudioManager* audio_manager = engine_context->get_audio_manager();
        if (audio_manager) {
            LatencyInfo latency = audio_manager->get_latency_info();
            if (latency.valid) {
                debugger->log(DebugLevel::INFO, 
                    "Audio latency: " + std::to_string(latency.period_size_in_milliseconds) + "ms, " +
                    "Sample rate: " + std::to_string(latency.sample_rate) + "hz, " +
                    "Period size: " + std::to_string(latency.period_size_in_frames) + " frames",
                    __FILE__, __LINE__);
            } else {
                debugger->log(DebugLevel::WARNING, "Could not retrieve audio latency info", __FILE__, __LINE__);
            }
        }

        std::string audioDir = getAssetPath("sounds/audio.mp3");

        Audio* loaded_audio = nullptr;
        AudioResult audioResult = audio_manager->load_audio(audioDir);
        
        if (audioResult.status == MA_SUCCESS && audioResult.audio != nullptr) {
            debugger->log(DebugLevel::INFO, 
                "Loaded " + audioDir + " successfully. Status: " + std::to_string(audioResult.status), 
                __FILE__, __LINE__);
            loaded_audio = audioResult.audio;

            AudioResult playResult = audio_manager->play_audio(audioResult.audio);
            if (playResult.status == MA_SUCCESS) {
                debugger->log(DebugLevel::INFO, 
                    "Audio playing successfully. Status: " + std::to_string(audioResult.status), 
                    __FILE__, __LINE__);

                audioResult.audio->set_volume(0.1f);
                audioResult.audio->set_looping(true);
                audioResult.audio->set_playback_rate(2.0f);
            } 
            else
            {
                debugger->log(DebugLevel::ERROR, 
                    "Audio failed to play. Status: " + std::to_string(playResult.status), 
                    __FILE__, __LINE__);
            }
        } else {
            debugger->log(DebugLevel::ERROR, 
                "Failed to load audio. Status: " + std::to_string(audioResult.status), 
                __FILE__, __LINE__);
        }
    }

    DebugScreen::~DebugScreen()
    {
        engine_context->get_debugger()->log(DebugLevel::INFO, "DebugScreen unloaded", __FILE__, __LINE__);
    }

    void DebugScreen::update(float delta_time)
    {
        float fps = getFPS(delta_time);
        std::string memory = getFormattedMemoryUsage();

        text_component.setText("FPS: " + std::to_string(static_cast<int>(fps)) + "\nMemory: " + memory);
    }

    void DebugScreen::render()
    {
        text_component.render();
        sprite_component.render();
    }
}
