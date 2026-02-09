#include "core/ui/spriteComponent.hpp"

#include "core/debug.hpp"


namespace vsrg {

SpriteComponent::SpriteComponent(EngineContext *engine_context, const std::string &spritePath)
    : UIComponent(engine_context), texture_path(spritePath) {
    auto *cached_texture = engine_context->get_texture_cache()->getTexture(spritePath);

    if (cached_texture && cached_texture->loaded) {
        dimensions = glm::vec2(cached_texture->dimensions);
        loaded = true;
    } else {
        engine_context->get_debugger()->log(
            DebugLevel::ERROR,
            std::string("Failed to create sprite component - texture not found: ") + spritePath,
            __FILE__, __LINE__);
        dimensions = glm::vec2(0.0f);
        loaded = false;
    }
}

SpriteComponent::~SpriteComponent() {}

glm::vec2 SpriteComponent::getSize() const {
    // If no render_size is set, use original texture dimensions
    if (properties.render_size.x <= 0.0f || properties.render_size.y <= 0.0f) {
        return dimensions;
    }

    // Stretch mode: ignore aspect ratio, use render_size as-is
    if (properties.render_mode == RenderMode::Stretch) {
        return properties.render_size;
    }

    // Calculate aspect ratios
    float texture_aspect = dimensions.x / dimensions.y;
    float target_aspect = properties.render_size.x / properties.render_size.y;

    glm::vec2 result;

    if (properties.render_mode == RenderMode::Fit) {
        // Fit mode: maintain aspect ratio, fit within render_size
        if (texture_aspect > target_aspect) {
            // Texture is wider, fit to width
            result.x = properties.render_size.x;
            result.y = properties.render_size.x / texture_aspect;
        } else {
            // Texture is taller, fit to height
            result.y = properties.render_size.y;
            result.x = properties.render_size.y * texture_aspect;
        }
    } else {  // RenderMode::Crop
        // Crop mode: maintain aspect ratio, fill render_size
        if (texture_aspect > target_aspect) {
            // Texture is wider, fit to height (crop width)
            result.y = properties.render_size.y;
            result.x = properties.render_size.y * texture_aspect;
        } else {
            // Texture is taller, fit to width (crop height)
            result.x = properties.render_size.x;
            result.y = properties.render_size.x / texture_aspect;
        }
    }

    return result;
}

void SpriteComponent::render() {
    if (!properties.visible) return;

    auto *renderer = engine_context->get_sprite_renderer();
    if (properties.use_custom_uv) {
        renderer->drawSprite(texture_path, properties.position, getSize(), properties.uv_rect,
                             properties.rotation, properties.anchor, properties.scale,
                             properties.opacity, properties.layer);
    } else {
        renderer->drawSprite(texture_path, properties.position, getSize(), properties.rotation,
                             properties.anchor, properties.scale, properties.opacity,
                             properties.layer);
    }
}

}  // namespace vsrg