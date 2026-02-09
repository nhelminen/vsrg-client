#include "core/ui/sprite.hpp"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/debug.hpp"
#include "core/engine/shader.hpp"


namespace vsrg {

const char *batch_vertex_shader = R"glsl(
    #version 330 core
    layout (location = 0) in vec2 position;
    layout (location = 1) in vec2 tex_coords;
    layout (location = 2) in float opacity;
    layout (location = 3) in float z_order;
    
    out vec2 v_tex_coords;
    out float v_opacity;
    
    uniform mat4 projection;
    
    void main() {
        gl_Position = projection * vec4(position, z_order, 1.0);
        v_tex_coords = tex_coords;
        v_opacity = opacity;
    }
)glsl";

const char *batch_fragment_shader = R"glsl(
    #version 330 core
    in vec2 v_tex_coords;
    in float v_opacity;
    
    out vec4 frag_color;
    
    uniform sampler2D sprite_texture;
    
    void main() {
        vec4 sampled = texture(sprite_texture, v_tex_coords);
        frag_color = vec4(sampled.rgb, sampled.a * v_opacity);
    }
)glsl";

SpriteRenderer::SpriteRenderer(EngineContext *engine_context) : engine_context(engine_context) {
    setupShader();
    setupBuffers();

    engine_context->get_debugger()->log(DebugLevel::INFO, "Sprite batch renderer initialized",
                                        __FILE__, __LINE__);
}

SpriteRenderer::~SpriteRenderer() {
    if (shader_program) glDeleteProgram(shader_program);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
}

void SpriteRenderer::setupShader() {
    shader_program =
        createShaderProgram(engine_context, batch_vertex_shader, batch_fragment_shader);

    projection_uniform = glGetUniformLocation(shader_program, "projection");
    texture_uniform = glGetUniformLocation(shader_program, "sprite_texture");
}

void SpriteRenderer::setupBuffers() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteVertex) * 6 * MAX_BATCH_SIZE, nullptr,
                 GL_DYNAMIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex),
                          (void *)offsetof(SpriteVertex, position));

    // tex_coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex),
                          (void *)offsetof(SpriteVertex, tex_coords));

    // opacity
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex),
                          (void *)offsetof(SpriteVertex, opacity));

    // z_order
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex),
                          (void *)offsetof(SpriteVertex, z_order));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SpriteRenderer::begin() {
    batches.clear();
}

void SpriteRenderer::drawSprite(const std::string &texture_path, const glm::vec2 &position,
                                const glm::vec2 &size, float rotation, const glm::vec2 &anchor,
                                const glm::vec2 &scale, float opacity, int layer) {
    auto *cached_texture = engine_context->get_texture_cache()->getTexture(texture_path);
    if (!cached_texture || !cached_texture->loaded || !cached_texture->texture_id) {
        return;
    }

    SpriteBatch *batch = getBatch(cached_texture->texture_id, layer);

    if (batch->sprite_count >= MAX_BATCH_SIZE) {
        flush();
        batch = getBatch(cached_texture->texture_id, layer);
    }

    glm::vec2 scaled_size = size * scale;
    glm::vec2 anchor_offset = scaled_size * anchor;

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    transform = glm::translate(transform, glm::vec3(-anchor_offset, 0.0f));

    if (rotation != 0.0f) {
        transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    if (scale != glm::vec2(1.0f)) {
        transform = glm::scale(transform, glm::vec3(scale, 1.0f));
    }

    glm::vec4 corners[4] = {transform * glm::vec4(0.0f, size.y, 0.0f, 1.0f),
                            transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
                            transform * glm::vec4(size.x, 0.0f, 0.0f, 1.0f),
                            transform * glm::vec4(size.x, size.y, 0.0f, 1.0f)};

    glm::vec2 tex_coords[4] = {{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}};

    float z_value = layer * 0.1f;  // Calculate z from layer (higher layer = closer to camera)

    batch->vertices.push_back({glm::vec2(corners[0]), tex_coords[0], opacity, z_value});
    batch->vertices.push_back({glm::vec2(corners[1]), tex_coords[1], opacity, z_value});
    batch->vertices.push_back({glm::vec2(corners[2]), tex_coords[2], opacity, z_value});

    batch->vertices.push_back({glm::vec2(corners[0]), tex_coords[0], opacity, z_value});
    batch->vertices.push_back({glm::vec2(corners[2]), tex_coords[2], opacity, z_value});
    batch->vertices.push_back({glm::vec2(corners[3]), tex_coords[3], opacity, z_value});

    batch->sprite_count++;
}

void SpriteRenderer::drawSprite(const std::string &texture_path, const glm::vec2 &position,
                                const glm::vec2 &size, const glm::vec4 &uv_rect, float rotation,
                                const glm::vec2 &anchor, const glm::vec2 &scale, float opacity,
                                int layer) {
    auto *cached_texture = engine_context->get_texture_cache()->getTexture(texture_path);
    if (!cached_texture || !cached_texture->loaded || !cached_texture->texture_id) {
        return;
    }

    SpriteBatch *batch = getBatch(cached_texture->texture_id, layer);

    if (batch->sprite_count >= MAX_BATCH_SIZE) {
        flush();
        batch = getBatch(cached_texture->texture_id, layer);
    }

    glm::vec2 scaled_size = size * scale;
    glm::vec2 anchor_offset = scaled_size * anchor;

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(position, 0.0f));
    transform = glm::translate(transform, glm::vec3(-anchor_offset, 0.0f));

    if (rotation != 0.0f) {
        transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    if (scale != glm::vec2(1.0f)) {
        transform = glm::scale(transform, glm::vec3(scale, 1.0f));
    }

    glm::vec4 corners[4] = {transform * glm::vec4(0.0f, size.y, 0.0f, 1.0f),
                            transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
                            transform * glm::vec4(size.x, 0.0f, 0.0f, 1.0f),
                            transform * glm::vec4(size.x, size.y, 0.0f, 1.0f)};

    // UV Rect: x, y, width, height (normalized 0-1)
    // Standard (0,0,1,1) maps to:
    // TL: {0,1}, BL: {0,0}, BR: {1,0}, TR: {1,1}
    // This seems inverted Y? OpenGL 0,0 is usually Bottom-Left.
    // In previous drawSprite:
    // corners[0] (0, size.y) -> tex_coords[0] {0.0f, 1.0f} (Top-Left geometry ->
    // Top-Left UV?) corners[1] (0, 0)      -> tex_coords[1] {0.0f, 0.0f}
    // (Bottom-Left geometry -> Bottom-Left UV) So Y=0 is Bottom. Y=1 is Top in
    // UVs.

    float u1 = uv_rect.x;
    float v1 = uv_rect.y;
    float u2 = uv_rect.x + uv_rect.z;
    float v2 = uv_rect.y + uv_rect.w;

    glm::vec2 tex_coords[4] = {{u1, v2}, {u1, v1}, {u2, v1}, {u2, v2}};

    float z_value = layer * 0.1f;

    batch->vertices.push_back({glm::vec2(corners[0]), tex_coords[0], opacity, z_value});
    batch->vertices.push_back({glm::vec2(corners[1]), tex_coords[1], opacity, z_value});
    batch->vertices.push_back({glm::vec2(corners[2]), tex_coords[2], opacity, z_value});

    batch->vertices.push_back({glm::vec2(corners[0]), tex_coords[0], opacity, z_value});
    batch->vertices.push_back({glm::vec2(corners[2]), tex_coords[2], opacity, z_value});
    batch->vertices.push_back({glm::vec2(corners[3]), tex_coords[3], opacity, z_value});

    batch->sprite_count++;
}

void SpriteRenderer::end() {
    flush();
}

void SpriteRenderer::flush() {
    if (batches.empty()) return;

    std::sort(batches.begin(), batches.end(),
              [](const SpriteBatch &a, const SpriteBatch &b) { return a.layer < b.layer; });

    glUseProgram(shader_program);

    glm::mat4 projection = glm::ortho(0.0f, (float)engine_context->get_screen_width(),
                                      (float)engine_context->get_screen_height(), 0.0f);
    glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(texture_uniform, 0);

    // render each batch
    for (auto &batch : batches) {
        if (batch.vertices.empty()) continue;

        glBindTexture(GL_TEXTURE_2D, batch.texture_id);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SpriteVertex) * batch.vertices.size(),
                        batch.vertices.data());

        glDrawArrays(GL_TRIANGLES, 0, batch.vertices.size());
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    batches.clear();
}

SpriteBatch *SpriteRenderer::getBatch(GLuint texture_id, int layer) {
    for (auto &batch : batches) {
        if (batch.texture_id == texture_id && batch.layer == layer &&
            batch.sprite_count < MAX_BATCH_SIZE) {
            return &batch;
        }
    }

    // create new batch
    SpriteBatch new_batch;
    new_batch.texture_id = texture_id;
    new_batch.layer = layer;
    batches.push_back(new_batch);
    return &batches.back();
}

void SpriteRenderer::drawSpriteImmediate(const std::string &texture_path, const glm::vec2 &position,
                                         const glm::vec2 &size, float rotation,
                                         const glm::vec2 &anchor, const glm::vec2 &scale,
                                         float opacity, int layer) {
    begin();
    drawSprite(texture_path, position, size, rotation, anchor, scale, opacity, layer);
    end();
}

}  // namespace vsrg