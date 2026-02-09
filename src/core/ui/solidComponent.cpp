#include "core/ui/solidComponent.hpp"

#include "core/debug.hpp"
#include "core/engine/shader.hpp"


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vsrg {
const char *color_vertex_shader = R"glsl(
    #version 330 core
    layout (location = 0) in vec2 vertex;

    uniform mat4 projection;
    uniform float z_order;

    void main()
    {
        gl_Position = projection * vec4(vertex.xy, z_order, 1.0);
    }
)glsl";

const char *color_frag_shader = R"glsl(
    #version 330 core
    out vec4 frag_color;
    
    uniform vec4 color;
    uniform float opacity;

    void main()
    {
        frag_color = vec4(color.rgb, color.a * opacity);
    }
)glsl";

SolidComponent::SolidComponent(EngineContext *engine_context, const glm::vec4 &color)
    : UIComponent(engine_context), color(color), dimensions(100.0f, 100.0f) {
    shader_program = createShaderProgram(engine_context, color_vertex_shader, color_frag_shader);

    projection_uniform = glGetUniformLocation(shader_program, "projection");
    opacity_uniform = glGetUniformLocation(shader_program, "opacity");
    color_uniform = glGetUniformLocation(shader_program, "color");
    z_order_uniform = glGetUniformLocation(shader_program, "z_order");

    setupBuffers();
}

SolidComponent::~SolidComponent() {
    glDeleteProgram(shader_program);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
}

void SolidComponent::setupBuffers() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SolidComponent::render() {
    if (!properties.visible || properties.opacity == 0.0f) return;

    glUseProgram(shader_program);
    glUniform1f(opacity_uniform, properties.opacity);
    glUniform4fv(color_uniform, 1, glm::value_ptr(color));

    // set z-order from layer
    float z_value = properties.layer * 0.1f;
    glUniform1f(z_order_uniform, z_value);

    glm::vec2 dims = (properties.render_size.x > 0.0f && properties.render_size.y > 0.0f)
                         ? properties.render_size
                         : dimensions;

    glm::vec2 anchor_offset = dims * properties.anchor;

    glm::mat4 projection = glm::ortho(0.0f, (float)engine_context->get_screen_width(),
                                      (float)engine_context->get_screen_height(), 0.0f);

    glm::mat4 transform = glm::mat4(1.0f);

    transform = glm::translate(transform, glm::vec3(properties.position, 0.0f));
    transform = glm::translate(transform, glm::vec3(-anchor_offset, 0.0f));

    if (properties.rotation != 0.0f) {
        transform =
            glm::rotate(transform, glm::radians(properties.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    if (properties.scale != glm::vec2(1.0f)) {
        transform = glm::scale(transform, glm::vec3(properties.scale, 1.0f));
    }

    glm::mat4 final_projection = projection * transform;
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(final_projection));

    float w = dims.x;
    float h = dims.y;

    float vertices[6][2] = {{0.0f, h}, {0.0f, 0.0f}, {w, 0.0f}, {0.0f, h}, {w, 0.0f}, {w, h}};

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);
}
}  // namespace vsrg