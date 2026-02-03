#include "core/textComponent.hpp"
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

    void main()
    {
        float alpha = texture(atlas, texture_coords).r;
        texture_color = vec4(color, alpha);
    }
)glsl";

	TextComponent::TextComponent(EngineContext* engine_context, Font* font, const std::string& text, const TextRenderOptions& render_options) : engine_context(engine_context), font(font), render_options(render_options) {
		setText(text);

		shader_program = createShaderProgram(engine_context, vertex_shader_source, fragment_shader_source);
		projection_uniform = glGetUniformLocation(shader_program, "projection");
		atlas_uniform = glGetUniformLocation(shader_program, "atlas");
		color_uniform = glGetUniformLocation(shader_program, "color");
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

	void TextComponent::setRenderOptions(const TextRenderOptions& options) {
		render_options = options;
		drawable_string.dimensions = calculateDimensions(drawable_string.text);
	}
		
	void TextComponent::render() {
		if (drawable_string.text.empty()) return;

		glUseProgram(shader_program);

		glUniform3f(color_uniform, render_options.color.x, render_options.color.y, render_options.color.z);

		glm::mat4 projection = glm::ortho(0.0f, (float)engine_context->get_screen_width(), (float)engine_context->get_screen_height(), 0.0f);
		glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(atlas_uniform, 0);
		glBindVertexArray(font->getVAO());

		float current_x = render_options.position.x;
		float scaling_factor = getScalingFactor();
		GLuint last_texture_id = 0;

		for (auto c = drawable_string.text.begin(); c != drawable_string.text.end(); c++) {
			auto ch = font->getCharacter(*c);
			if (!ch) continue;

			// if the character is on a different atlas, bind the new texture
			if (ch->texture_id != last_texture_id) {
				glBindTexture(GL_TEXTURE_2D, ch->texture_id);
				last_texture_id = ch->texture_id;
			}

			float xpos = current_x + ch->bearing.x * scaling_factor;
			float ypos = render_options.position.y + (font->getBaselineHeight() - ch->bearing.y + font->getSizePt()) * scaling_factor;
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
		return render_options.size / font->getSizePt();
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

		float total_width = 0.0f;
		int max_ascent = 0;
		int min_descent = 0;
		bool has_visible_glyphs = false;

		for (auto c : text) {
			auto ch = font->getCharacter(c);
			if (!ch) continue;

			total_width += ch->advance * getScalingFactor();

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

		float total_height = has_visible_glyphs ? (max_ascent - min_descent) * getScalingFactor() : 0.0f;

		return { total_width, total_height };
	}

}