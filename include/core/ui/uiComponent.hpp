#pragma once

#include "public/engineContext.hpp"
#include "core/shader.hpp"

namespace vsrg {
    struct ComponentProperties {
        bool visible = true;
        int z_order = 1;

        float opacity = 1.0f;
        float rotation = 0.0f;

		glm::vec2 position = glm::vec2(0.0f);
        glm::vec2 scale = glm::vec2(1.0f);
        glm::vec2 anchor = glm::vec2(0.0f);
	};

    class UIComponent {
	public:
        UIComponent(EngineContext* engine_context) : engine_context(engine_context) {}
        virtual ~UIComponent() = default;

        virtual void render() = 0;
        virtual glm::vec2 getDimensions() const = 0;

        virtual void setPosition(const glm::vec2& pos) { properties.position = pos; }
        virtual glm::vec2 getPosition() const { return properties.position; }

        virtual void setOpacity(float opacity) { 
            properties.opacity = glm::clamp(opacity, 0.0f, 1.0f); 
        }
        virtual float getOpacity() const { return properties.opacity; }
        
        virtual void setVisible(bool visible) { properties.visible = visible; }
        virtual bool isVisible() const { return properties.visible; }

        virtual void setScale(const glm::vec2& scale) { properties.scale = scale; }
        virtual glm::vec2 getScale() const { return properties.scale; }

        virtual void setZOrder(int z) { properties.z_order = z; }
        virtual int getZOrder() const { return properties.z_order; }

        virtual void setRotation(float rotation) { properties.rotation = glm::radians(rotation); }
        virtual float getRotation() const { return glm::degrees(properties.rotation); }

        ComponentProperties& getProperties() { return properties; }
        void setProperties(ComponentProperties _properties) { properties = _properties; }
    protected:
        EngineContext* engine_context;
        ComponentProperties properties;
    };
}