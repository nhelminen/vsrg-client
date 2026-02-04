#include "core/screens/debugScreen.hpp"
#include "core/engine/shader.hpp"
#include "core/engine/audio.hpp"
#include "core/engine/plugin.hpp"
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
            1.0f, 0.0f,
            { 16.0f, 16.0f },
            { 1.0f, 1.0f },
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
            { 16.0f, 160.0f },
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
        AudioResult audioResult = audio_manager->load_audio(audioDir);
        
        if (audioResult.status == MA_SUCCESS && audioResult.audio != nullptr) {
            debugger->log(DebugLevel::INFO, 
                "Loaded " + audioDir + " successfully. Status: " + std::to_string(audioResult.status), 
                __FILE__, __LINE__);
            loaded_audio = audioResult.audio;
            audioResult.audio->set_volume(0.1f);

            conductor = new Conductor(audio_manager, loaded_audio, {
                { 0.0f, 220.0f, 4, 4 }
            });
            conductor->play();
        } else {
            debugger->log(DebugLevel::ERROR, 
                "Failed to load audio. Status: " + std::to_string(audioResult.status), 
                __FILE__, __LINE__);
        }

        IGamePlugin* plugin = engine_context->get_plugin_manager()->find_plugin("mania");
	if (plugin != nullptr) {
        	engine_context->get_plugin_manager()->activate_plugin(plugin->get_info().name);
        	plugin->load();
	} 
	else
	{
	    engine_context->get_debugger()->log(DebugLevel::INFO, "plugin doesnt exist", __FILE__, __LINE__);
	}
    }

    DebugScreen::~DebugScreen()
    {
        engine_context->get_debugger()->log(DebugLevel::INFO, "DebugScreen unloaded", __FILE__, __LINE__);
    }

    void DebugScreen::update(float delta_time)
    {
        if (conductor) 
        {
            conductor->update(delta_time);
        }

        float fps = getFPS(delta_time);
        std::string memory = getFormattedMemoryUsage();

        std::stringstream textData;
        textData << "FPS: " << static_cast<int>(fps) << "\n";
        textData << "Memory: " << memory << "\n";

        if (conductor != nullptr) 
        {
            glm::vec2 timeSig = conductor->get_time_signature();
            
            textData << "\nBPM: " << conductor->get_bpm() << "\n";
            textData << "Time: " << std::fixed << std::setprecision(2) << conductor->get_song_position();
            textData << " / " << conductor->get_song_duration() << "\n";
            textData << "Signature: " << static_cast<int>(timeSig.x) << "/" << static_cast<int>(timeSig.y) << "\n\n";

            textData << "Beat: " << conductor->get_beat() << "\n";
            textData << "Step: " << conductor->get_step() << "\n";
        }

        text_component.setText(textData.str());
    }

    void DebugScreen::render()
    {
        text_component.render();
        sprite_component.render();
    }
}
