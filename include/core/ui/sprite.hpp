#pragma once

#include <glad/glad.h>

#include "core/ui/texture.hpp"
#include "public/engineContext.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <string>
#include <vector>


namespace vsrg {
struct SpriteVertex {
    glm::vec2 position;
    glm::vec2 tex_coords;
    float opacity;
    float z_order;
};

struct SpriteBatch {
    GLuint texture_id;
    int layer;
    std::vector<SpriteVertex> vertices;
    size_t sprite_count = 0;
};

class SpriteRenderer {
public:
    SpriteRenderer(EngineContext *engine_context);
    ~SpriteRenderer();

    void begin();

    void drawSprite(const std::string &texture_path, const glm::vec2 &position,
                    const glm::vec2 &size, float rotation = 0.0f,
                    const glm::vec2 &anchor = glm::vec2(0.0f),
                    const glm::vec2 &scale = glm::vec2(1.0f), float opacity = 1.0f, int layer = 0);

    void drawSprite(const std::string &texture_path, const glm::vec2 &position,
                    const glm::vec2 &size, const glm::vec4 &uv_rect, float rotation = 0.0f,
                    const glm::vec2 &anchor = glm::vec2(0.0f),
                    const glm::vec2 &scale = glm::vec2(1.0f), float opacity = 1.0f, int layer = 0);

    void drawSpriteImmediate(const std::string &texture_path, const glm::vec2 &position,
                             const glm::vec2 &size, float rotation = 0.0f,
                             const glm::vec2 &anchor = glm::vec2(0.0f),
                             const glm::vec2 &scale = glm::vec2(1.0f), float opacity = 1.0f,
                             int layer = 0);

    void end();
    void flush();

private:
    void setupShader();
    void setupBuffers();

    SpriteBatch *getBatch(GLuint texture_id, int layer);

    EngineContext *engine_context;

    GLuint shader_program = 0;
    GLuint vao = 0;
    GLuint vbo = 0;

    GLint projection_uniform;
    GLint texture_uniform;

    std::vector<SpriteBatch> batches;

    static constexpr size_t MAX_BATCH_SIZE = 1000;
};
}  // namespace vsrg