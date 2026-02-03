#include "core/screens/TestScreen.hpp"
#include "core/debug.hpp"
#include "core/shader.hpp"
#include "core/audio.hpp"
#include "core/utils.hpp"

#include "public/engineContext.hpp"

#include <glad/glad.h>

const char *vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        
        void main()
        {
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

const char *fragment_shader_source = R"(
        #version 330 core
        out vec4 FragColor;
        
        uniform vec4 uColor;
        
        void main()
        {
            FragColor = uColor;
        }
    )";

namespace vsrg
{
    TestScreen::TestScreen(EngineContext* engine_context)
        : Screen(engine_context, "TestScreen", 1)
    {
        shader_program = createShaderProgram(engine_context, vertex_shader_source, fragment_shader_source);

        float vertices[] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, 0.5f
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        Debugger* debugger = engine_context->get_debugger();
        debugger->log(DebugLevel::INFO, "TestScreen loaded", __FILE__, __LINE__);

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

        std::string audioDir = getAssetPath("audio.mp3");

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

    TestScreen::~TestScreen()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteProgram(shader_program);

        engine_context->get_debugger()->log(DebugLevel::INFO, "TestScreen unloaded", __FILE__, __LINE__);
    }

    void TestScreen::update(float delta_time)
    {
        // update, use this for any time-based changes
    }

    void TestScreen::render()
    {
        glUseProgram(shader_program);
        
        GLint color_location = glGetUniformLocation(shader_program, "uColor");
        glUniform4f(color_location, 1.0f, 0.0f, 0.0f, 1.0f);
        
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);
        
        glUseProgram(0);
    }
}