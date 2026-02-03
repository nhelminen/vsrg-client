#include "core/ui/textComponent.hpp"
#include "core/shader.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vsrg {

	const char* vertex_shader_source = R"glsl(
    #version 330 core
    layout (location = 0) in vec4 vertex; // vec2 pos, vec2 tex
    out vec2 texture_coords;

    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
        texture_coords = vertex.zw;
    }
)glsl";

	const char* fragment_shader_source = R"glsl(
    #version 330 core
    in vec2 texture_coords;
    out vec4 texture_color;

    uniform sampler2D atlas;
    uniform vec3 color;
	uniform float opacity;

    void main()
    {
        float alpha = texture(atlas, texture_coords).r;
        texture_color = vec4(color, alpha * opacity);
    }
)glsl";

	TextComponent::TextComponent(EngineContext* engine_context, Font* font, const std::string& text, const TextRenderOptions& text_options) 
		: UIComponent(engine_context), font(font), text_options(text_options) {
		setText(text);

		shader_program = createShaderProgram(engine_context, vertex_shader_source, fragment_shader_source);
		projection_uniform = glGetUniformLocation(shader_program, "projection");
		atlas_uniform = glGetUniformLocation(shader_program, "atlas");
		color_uniform = glGetUniformLocation(shader_program, "color");
		opacity_uniform = glGetUniformLocation(shader_program, "opacity");
	}

	TextComponent::~TextComponent() {
		glDeleteProgram(shader_program);
	}

	void TextComponent::setText(const std::string& text) {
		std::vector<Charcode> charcodes = decodeUTF8(text);
		glm::vec2 dimensions = calculateDimensions(charcodes);
		drawable_string = DrawableString{ charcodes, dimensions };
	}

	void TextComponent::setFont(Font* new_font) {
		font = new_font;
		drawable_string.dimensions = calculateDimensions(drawable_string.text);
	}

	void TextComponent::setTextOptions(const TextRenderOptions& options) {
		text_options = options;
		drawable_string.dimensions = calculateDimensions(drawable_string.text);
	}
		
	void TextComponent::render() {
		if (!properties.visible || properties.opacity == 0.0f || drawable_string.text.empty()) return;

		glUseProgram(shader_program);

		glUniform1f(opacity_uniform, properties.opacity);
    	glUniform3f(color_uniform, text_options.color.x, text_options.color.y, text_options.color.z);

		glm::vec2 dims = getDimensions();
    	glm::vec2 anchor_offset = dims * properties.anchor;

		glm::mat4 projection = glm::ortho(
			0.0f, (float)engine_context->get_screen_width(), 
			(float)engine_context->get_screen_height(), 0.0f
		);

		glm::mat4 transform = glm::mat4(1.0f);

		transform = glm::translate(transform, glm::vec3(properties.position, 0.0f));
		transform = glm::translate(transform, glm::vec3(-anchor_offset, 0.0f));

		if (properties.rotation != 0.0f) {
			transform = glm::rotate(transform, glm::radians(properties.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		if (properties.scale != glm::vec2(1.0f)) {
			transform = glm::scale(transform, glm::vec3(properties.scale, 1.0f));
		}

		glm::mat4 final_projection = projection * transform;
    	glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(final_projection));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(atlas_uniform, 0);
		glBindVertexArray(font->getVAO());

		float current_x = 0.0f;
		float current_y = 0.0f;

		float scaling_factor = getScalingFactor();
		float line_height = (font->getSizePt() + text_options.line_gap) * scaling_factor;
		GLuint last_texture_id = 0;

		for (auto c = drawable_string.text.begin(); c != drawable_string.text.end(); c++) {
			if (*c == '\n') {
				current_x = 0.0f;
				current_y += line_height;
				continue;
			}

			auto ch = font->getCharacter(*c);
			if (!ch) continue;

			// if the character is on a different atlas, bind the new texture
			if (ch->texture_id != last_texture_id) {
				glBindTexture(GL_TEXTURE_2D, ch->texture_id);
				last_texture_id = ch->texture_id;
			}

			float xpos = current_x + ch->bearing.x * scaling_factor;
			float ypos = current_y + (font->getBaselineHeight() - ch->bearing.y + font->getSizePt()) * scaling_factor;
			float w = ch->size.x * scaling_factor;
			float h = ch->size.y * scaling_factor;

			float vertices[6][4] = {
				{xpos, ypos + h, ch->atlas_offset.x, ch->atlas_offset.y + ch->atlas_size_norm.y},
				{xpos, ypos, ch->atlas_offset.x, ch->atlas_offset.y},
				{xpos + w, ypos, ch->atlas_offset.x + ch->atlas_size_norm.x, ch->atlas_offset.y},
				{xpos, ypos + h, ch->atlas_offset.x, ch->atlas_offset.y + ch->atlas_size_norm.y},
				{xpos + w, ypos, ch->atlas_offset.x + ch->atlas_size_norm.x, ch->atlas_offset.y},
				{xpos + w, ypos + h, ch->atlas_offset.x + ch->atlas_size_norm.x, ch->atlas_offset.y + ch->atlas_size_norm.y}
			};

			glBindBuffer(GL_ARRAY_BUFFER, font->getVBO());
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			current_x += ch->advance * scaling_factor;
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	float TextComponent::getScalingFactor() const {
		return text_options.size / font->getSizePt();
	}

	std::vector<Charcode> TextComponent::decodeUTF8(const std::string& str) {
		std::vector<FT_ULong> codepoints;
		const unsigned char* s = reinterpret_cast<const unsigned char*>(str.c_str());
		const unsigned char* end = s + str.length();

		// vibe coded btw don't judge
		while (s < end) {
			FT_ULong codepoint = 0;
			unsigned char c = *s++;

			if (c < 0x80) {
				codepoint = c;
			}
			else if ((c & 0xE0) == 0xC0) {
				if (s < end && (*s & 0xC0) == 0x80) {
					codepoint = ((c & 0x1F) << 6) | (*s & 0x3F);
					s++;
				}
				else { codepoint = '?'; }
			}
			else if ((c & 0xF0) == 0xE0) {
				if (s + 1 < end && (*s & 0xC0) == 0x80 && (*(s + 1) & 0xC0) == 0x80) {
					codepoint = ((c & 0x0F) << 12) | ((*s & 0x3F) << 6) | (*(s + 1) & 0x3F);
					s += 2;
				}
				else { codepoint = '?'; }
			}
			else if ((c & 0xF8) == 0xF0) {
				if (s + 2 < end && (*s & 0xC0) == 0x80 && (*(s + 1) & 0xC0) == 0x80 && (*(s + 2) & 0xC0) == 0x80) {
					codepoint = ((c & 0x07) << 18) | ((*s & 0x3F) << 12) | ((*(s + 1) & 0x3F) << 6) | (*(s + 2) & 0x3F);
					s += 3;
				}
				else { codepoint = '?'; }
			}
			else {
				codepoint = '?';
			}
			codepoints.push_back(codepoint);
		}
		return codepoints;
	}

	glm::vec2 TextComponent::calculateDimensions(const std::vector<Charcode>& text) {
		if (text.empty()) {
			return { 0, 0 };
		}

		float current_line_width = 0.0f;
		float max_width = 0.0f;
		int max_ascent = 0;
		int min_descent = 0;
		bool has_visible_glyphs = false;
		int line_count = 1;

		for (auto c : text) {
			if (c == '\n') {
				max_width = std::max(max_width, current_line_width);
				current_line_width = 0.0f;
				line_count++;
				continue;
			}

			auto ch = font->getCharacter(c);
			if (!ch) continue;

			current_line_width += ch->advance * getScalingFactor();

			if (ch->size.x > 0 && ch->size.y > 0) {
				if (!has_visible_glyphs) {
					max_ascent = ch->bearing.y;
					min_descent = ch->bearing.y - ch->size.y;
					has_visible_glyphs = true;
				}
				else {
					max_ascent = std::max(max_ascent, (int)ch->bearing.y);
					min_descent = std::min(min_descent, (int)(ch->bearing.y - ch->size.y));
				}
			}
		}

		max_width = std::max(max_width, current_line_width);

		float line_height = (font->getSizePt() + text_options.line_gap) * getScalingFactor();
		float total_height = line_count * line_height;

		return { max_width, total_height };
	}

}