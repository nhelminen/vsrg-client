#pragma once

#include "public/engineContext.hpp"
#include "core/ui/uiComponent.hpp"

#include <memory>
#include <string>

namespace vsrg {
    class SpriteComponent : public UIComponent {
	public:
		SpriteComponent(EngineContext* engine_context, const std::string& spritePath);
		~SpriteComponent();

        void render() override;
		glm::vec2 getDimensions() const override { return dimensions; }
    private:
        void loadTexture(const std::string& path);
		void setupBuffers();

        GLuint shader_program;
		GLuint texture_id;
		GLuint vao;
		GLuint vbo;

		GLint projection_uniform;
        GLint texture_uniform;
		GLint opacity_uniform;

		glm::vec2 dimensions;
    };
}