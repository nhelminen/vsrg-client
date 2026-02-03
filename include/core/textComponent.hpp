#pragma once

#include "public/engineContext.hpp"
#include "core/font.hpp"

#include <vector>
#include <memory>

namespace vsrg {

	// TODO: more options like shadows, gradient, etc.
	struct TextRenderOptions {
		glm::vec2 position = glm::vec2(0.0f);
		float size = 1.0f;
		glm::vec3 color = glm::vec3(1.0f);
	};

	class TextComponent {
	public:
		TextComponent(EngineContext* engine_context, Font* font, const std::string& text = "", const TextRenderOptions& render_options = TextRenderOptions{});
		~TextComponent();

		void setText(const std::string& text);
		void setFont(Font* new_font);
		void setRenderOptions(const TextRenderOptions& options);
		void render();

		glm::vec2 getDimensions() const { return drawable_string.dimensions; }

	private:
		struct DrawableString {
			std::vector<Charcode> text;
			glm::vec2 dimensions = glm::vec2(0.0f);
		};

		float getScalingFactor() const;
		std::vector<Charcode> decodeUTF8(const std::string& str);
		glm::vec2 calculateDimensions(const std::vector<Charcode>& text);

		EngineContext* engine_context;
		Font* font;
		GLuint shader_program;
		GLint projection_uniform;
		GLint atlas_uniform;
		GLint color_uniform;
		TextRenderOptions render_options;
		DrawableString drawable_string;
	};

}