#pragma once

#include "public/engineContext.hpp"

#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <optional>
#include <vector>


namespace vsrg {

	using Charcode = FT_ULong;

	const unsigned int FONT_ATLAS_DEFAULT_SIZE = 2048;

	struct Character {
		GLuint texture_id;
		glm::ivec2 size;
		glm::ivec2 bearing;
		GLuint advance;
		glm::vec2 atlas_offset;
		glm::vec2 atlas_size_norm;
	};

	class FontLoader {
	public:
		FontLoader(EngineContext* engine_context);
		~FontLoader();

		Font loadFont(const std::string& path, unsigned int size_pt);

	private:
		EngineContext* engine_context;
		FT_Library ft;
	};

	class Font {
	public:
		Font(EngineContext* engine_context, FT_Library ft, const std::string& path, unsigned int size_pt);
		~Font();

		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;

		bool isLoaded() const { return loaded; }
		GLuint getVAO() const { return VAO; }
		GLuint getVBO() const { return VBO; }
		unsigned int getSizePt() const { return size_pt; }
		unsigned int getBaselineHeight() const { return baseline_height; }

		std::optional<Character> getCharacter(Charcode charcode);

	private:
		struct Atlas {
			GLuint texture_id = 0;
			unsigned int width = 0;
			unsigned int height = 0;
			unsigned int current_x = 0;
			unsigned int current_y = 0;
			unsigned int max_row_height = 0;
		};

		bool createNewAtlas();
		bool initBuffers();
		bool loadGlyph(Charcode charcode);

		EngineContext* engine_context;
		bool loaded = false;
		FT_Face face = nullptr;
		unsigned int size_pt = 0;
		unsigned int baseline_height = 0;
		GLuint VAO = 0;
		GLuint VBO = 0;
		std::vector<Atlas> atlases;
		std::unordered_map<Charcode, Character> characters;
	};

}