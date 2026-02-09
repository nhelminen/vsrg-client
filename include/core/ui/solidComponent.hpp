#pragma once

#include "core/ui/uiComponent.hpp"
#include "public/engineContext.hpp"

namespace vsrg {
class SolidComponent : public UIComponent {
public:
    SolidComponent(EngineContext *engine_context, const glm::vec4 &color);
    virtual ~SolidComponent();

    void render() override;

    glm::vec2 getSize() const override { return dimensions; }

    void setColor(const glm::vec4 &color) { this->color = color; }
    glm::vec4 getColor() const { return color; }

private:
    void setupBuffers();

    GLuint shader_program = 0;
    GLuint vao = 0;
    GLuint vbo = 0;

    GLint projection_uniform;
    GLint color_uniform;
    GLint opacity_uniform;
    GLint z_order_uniform;

    glm::vec2 dimensions;
    glm::vec4 color;
};
}  // namespace vsrg