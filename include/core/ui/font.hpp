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
	const int FONT_DEFAULT_SIZE_PT = 32;

	struct Character {
		GLuint texture_id;
		glm::ivec2 size;
		glm::ivec2 bearing;
		GLuint advance;
		glm::vec2 atlas_offset;
		glm::vec2 atlas_size_norm;
	};

	class Font {
	public:
		Font(EngineContext* engine_context, FT_Library ft, const std::string& path, int size_pt);
		~Font();

		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;

		Font(Font&&) noexcept = default;
		Font& operator=(Font&&) noexcept = default;

		bool isLoaded() const { return loaded; }
		GLuint getVAO() const { return VAO; }
		GLuint getVBO() const { return VBO; }
		int getSizePt() const { return size_pt; }
		int getBaselineHeight() const { return baseline_height; }
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
		int size_pt = 0;
		int baseline_height = 0;
		GLuint VAO = 0;
		GLuint VBO = 0;
		std::vector<Atlas> atlases;
		std::unordered_map<Charcode, Character> characters;
	};

	class FontManager {
	public:
		FontManager(EngineContext* engine_context);
		~FontManager();

		bool loadFont(const std::string& name, int size_pt = FONT_DEFAULT_SIZE_PT);
		Font* getFont(const std::string& name);

	private:
		EngineContext* engine_context;
		FT_Library ft;
		std::unordered_map<std::string, Font> fonts;
	};

}