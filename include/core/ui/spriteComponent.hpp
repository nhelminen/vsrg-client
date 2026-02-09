#pragma once

#include <memory>
#include <string>

#include "core/ui/sprite.hpp"
#include "core/ui/texture.hpp"
#include "core/ui/uiComponent.hpp"
#include "public/engineContext.hpp"


namespace vsrg {
class SpriteComponent : public UIComponent {
public:
    SpriteComponent(EngineContext *engine_context, const std::string &spritePath);
    virtual ~SpriteComponent();

    void render() override;
    glm::vec2 getSize() const override;

    bool isLoaded() const { return loaded; }

private:
    std::string texture_path;
    glm::vec2 dimensions;
    bool loaded = false;
};
}  // namespace vsrg