#pragma once

#include <glm/glm.hpp>

#include "core/engine/shader.hpp"
#include "public/engineContext.hpp"


namespace vsrg {
enum class RenderMode { Fit, Stretch, Crop };

struct ComponentProperties {
    bool visible = true;

    float opacity = 1.0f;
    float rotation = 0.0f;
    int layer = 0;

    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 scale = glm::vec2(1.0f);
    glm::vec2 anchor = glm::vec2(0.0f);
    glm::vec2 render_size = glm::vec2(0.0f);
    RenderMode render_mode = RenderMode::Fit;
    glm::vec4 uv_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);  // default full texture
    bool use_custom_uv = false;
};

class UIComponent {
public:
    UIComponent(EngineContext *ctx) : engine_context(ctx) {}
    virtual ~UIComponent() = default;

    virtual void update(float delta_time) {}
    virtual void render() {}

    void setVisible(bool visible) { properties.visible = visible; }
    void setOpacity(float opacity) { properties.opacity = opacity; }
    virtual void setRotation(float rotation) { properties.rotation = rotation; }
    virtual void setPosition(const glm::vec2 &pos) { properties.position = pos; }
    virtual void setScale(const glm::vec2 &scale) { properties.scale = scale; }
    virtual void setAnchor(const glm::vec2 &anchor) { properties.anchor = anchor; }
    virtual void setLayer(int layer) { properties.layer = layer; }

    virtual void setDimensions(const glm::vec2 &size) { properties.render_size = size; }

    // alias cause idk?
    void setSize(const glm::vec2 &size) { setDimensions(size); }

    virtual void setRenderMode(RenderMode mode) { properties.render_mode = mode; }
    void setUVRect(const glm::vec4 &uv) {
        properties.uv_rect = uv;
        properties.use_custom_uv = true;
    }
    void setUseCustomUV(bool use) { properties.use_custom_uv = use; }

    bool isVisible() const { return properties.visible; }
    float getOpacity() const { return properties.opacity; }
    virtual float getRotation() const { return properties.rotation; }
    virtual glm::vec2 getPosition() const { return properties.position; }
    virtual glm::vec2 getScale() const { return properties.scale; }
    virtual glm::vec2 getAnchor() const { return properties.anchor; }
    virtual int getLayer() const { return properties.layer; }
    virtual glm::vec2 getSize() const { return properties.render_size; }
    virtual RenderMode getRenderMode() const { return properties.render_mode; }

    ComponentProperties &getProperties() { return properties; }
    const ComponentProperties &getProperties() const { return properties; }
    void setProperties(const ComponentProperties &_properties) { properties = _properties; }

protected:
    EngineContext *engine_context;
    ComponentProperties properties;
};
}  // namespace vsrg