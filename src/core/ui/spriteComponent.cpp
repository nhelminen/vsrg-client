#include "core/ui/spriteComponent.hpp"
#include "core/engine/shader.hpp"
#include "core/debug.hpp"
#include "core/utils.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION_DONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION_DONE
#endif
#include <stb_image.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vsrg {
	const char* sprite_vertex_shader = R"glsl(
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

	const char* sprite_frag_shader = R"glsl(
    #version 330 core
    in vec2 texture_coords;
    out vec4 texture_color;
	
    uniform sampler2D sprite_texture;
	uniform float opacity;

    void main()
    {
        vec4 sampled = texture(sprite_texture, texture_coords);
        texture_color = vec4(sampled.rgb, sampled.a * opacity);
    }
)glsl";

	SpriteComponent::SpriteComponent(EngineContext* engine_context, const std::string& spritePath) 
		: UIComponent(engine_context) {
		shader_program = createShaderProgram(engine_context, sprite_vertex_shader, sprite_frag_shader);

		projection_uniform = glGetUniformLocation(shader_program, "projection");
		opacity_uniform = glGetUniformLocation(shader_program, "opacity");
		texture_uniform = glGetUniformLocation(shader_program, "sprite_texture");

		setupBuffers();
		loadTexture(spritePath);
	}

	SpriteComponent::~SpriteComponent() {
		glDeleteProgram(shader_program);
		if (texture_id) glDeleteTextures(1, &texture_id);
		if (vao) glDeleteVertexArrays(1, &vao);
		if (vbo) glDeleteBuffers(1, &vbo);
	}

	void SpriteComponent::setupBuffers() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void SpriteComponent::loadTexture(const std::string& path) {
		std::string execPath = getExecutableDir();
		std::string filePath = joinPaths(execPath, "assets", path);

		if (texture_id) {
			glDeleteTextures(1, &texture_id);
			texture_id = 0;
		}

		int width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

		if (data) {
			dimensions = glm::vec2(width, height);

			glGenTextures(1, &texture_id);
			glBindTexture(GL_TEXTURE_2D, texture_id);

			GLenum format = GL_RGB;
			if (channels == 1) format = GL_RED;
			else if (channels == 3) format = GL_RGB;
			else if (channels == 4) format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			engine_context->get_debugger()->log(DebugLevel::INFO,
				std::string("Successfully loaded sprite from path " + path),
				__FILE__, __LINE__);

			stbi_image_free(data);
		} else {
			engine_context->get_debugger()->log(DebugLevel::ERROR,
				std::string("Failed to load sprite from path " + path),
				__FILE__, __LINE__);
			dimensions = glm::vec2(0.0f);
		}
	}

	void SpriteComponent::render() {
		if (!properties.visible || properties.opacity == 0.0f || !texture_id) return;

		glUseProgram(shader_program);
		glUniform1f(opacity_uniform, properties.opacity);

		glm::vec2 dims = (properties.render_size.x > 0.0f && properties.render_size.y > 0.0f) 
			? properties.render_size 
			: dimensions;

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

		float w = dims.x;
		float h = dims.y;

		float vertices[6][4] = {
			{0.0f, h, 0.0f, 1.0f},
			{0.0f, 0.0f, 0.0f, 0.0f},
			{w, 0.0f, 1.0f, 0.0f},
			{0.0f, h, 0.0f, 1.0f},
			{w, 0.0f, 1.0f, 0.0f},
			{w, h, 1.0f, 1.0f}
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glUniform1i(texture_uniform, 0);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}
}