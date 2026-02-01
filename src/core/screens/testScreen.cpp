#include "core/screens/TestScreen.hpp"
#include "core/app.hpp"
#include "core/debug.hpp"

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
    TestScreen::TestScreen(Client *client)
        : Screen(client, "TestScreen", 1)
    {
        // why are we using 3.3 core??????? cant use immediate mode and we gotta use shaders what the hell
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);

        int success;
        char info_log[512];
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
            client->get_debugger()->log(DebugLevel::ERROR,
                                        std::string("Vertex shader compilation failed: ") + info_log,
                                        __FILE__, __LINE__);
        }

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);

        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
            client->get_debugger()->log(DebugLevel::ERROR,
                                        std::string("Fragment shader compilation failed: ") + info_log,
                                        __FILE__, __LINE__);
        }

        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
            client->get_debugger()->log(DebugLevel::ERROR,
                                        std::string("Shader program linking failed: ") + info_log,
                                        __FILE__, __LINE__);
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

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

        client->get_debugger()->log(DebugLevel::INFO, "TestScreen loaded", __FILE__, __LINE__);
    }

    TestScreen::~TestScreen()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteProgram(shader_program);

        client->get_debugger()->log(DebugLevel::INFO, "TestScreen unloaded", __FILE__, __LINE__);
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