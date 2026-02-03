#include "core/screens/TestScreen.hpp"
#include "core/debug.hpp"
#include "core/shader.hpp"

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
        : Screen(engine_context, "TestScreen", 1),
        font_manager(engine_context),
        font(engine_context, font_manager.getFt(), "assets/NotoSansJP-Regular.ttf", 32),
        text_component(engine_context, &font)
    {
        TextRenderOptions render_options = { {100.0f, 100.0f}, 32.0f, { 1.0f, 1.0f, 1.0f } };
        text_component.setText("\343\201\223\343\202\223\343\201\253\343\201\241\343\201\257\343\200\201\344\270\226\347\225\214");
        text_component.setRenderOptions(render_options);

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

        engine_context->get_debugger()->log(DebugLevel::INFO, "TestScreen loaded", __FILE__, __LINE__);
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

        text_component.render();
    }
}