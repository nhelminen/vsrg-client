#include "core/ui/font.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <tuple>
#include <utility>

#include "core/debug.hpp"
#include "core/utils.hpp"

namespace vsrg {

FontManager::FontManager(EngineContext* engine_context) : engine_context(engine_context) {
    if (FT_Init_FreeType(&ft)) {
        engine_context->get_debugger()->log(
            DebugLevel::ERROR, std::string("Failed to initialize FreeType."), __FILE__, __LINE__);
    }
    loadFont("NotoSansJP-Regular.ttf", FONT_DEFAULT_SIZE_PT);
}

FontManager::~FontManager() {
    fonts.clear();
    FT_Done_FreeType(ft);
}

bool FontManager::loadFont(const std::string& name, int size_pt) {
    std::string execPath = getExecutableDir();
    std::string filePath = joinPaths(execPath, "assets/fonts", name);

    auto [it, inserted] =
        fonts.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                      std::forward_as_tuple(engine_context, ft, filePath, size_pt));
    return it->second.isLoaded();
}

Font* FontManager::getFont(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end())
        return &it->second;
    else {
        if (loadFont(name)) {
            return &fonts.at(name);
        } else {
            return nullptr;
        }
    }
}

Font::Font(EngineContext* engine_context, FT_Library ft, const std::string& path, int size_pt)
    : engine_context(engine_context), size_pt(size_pt) {
    if (FT_New_Face(ft, path.c_str(), 0, &face)) {
        engine_context->get_debugger()->log(
            DebugLevel::ERROR, std::string("Failed to load font: ") + path, __FILE__, __LINE__);
        return;
    } else {
        engine_context->get_debugger()->log(DebugLevel::INFO, std::string("Font loaded: ") + path,
                                            __FILE__, __LINE__);
        engine_context->get_debugger()->log(
            DebugLevel::INFO, std::string(" Family: ") + face->family_name, __FILE__, __LINE__);
        engine_context->get_debugger()->log(
            DebugLevel::INFO, std::string(" Style: ") + face->style_name, __FILE__, __LINE__);
        engine_context->get_debugger()->log(
            DebugLevel::INFO, std::string(" Glyphs: ") + std::to_string(face->num_glyphs), __FILE__,
            __LINE__);
    }

    if (FT_Set_Char_Size(face, 0, size_pt * 64, 96, 96)) {
        engine_context->get_debugger()->log(DebugLevel::ERROR,
                                            std::string("Failed to set font size for ") + path,
                                            __FILE__, __LINE__);
        FT_Done_Face(face);
        face = nullptr;
        return;
    }

    if (!createNewAtlas()) return;
    if (!initBuffers()) return;

    for (Charcode c = 32; c < 128; c++) {
        loadGlyph(c);
    }

    if (auto H = getCharacter('H')) {
        baseline_height = H->bearing.y;
    }

    loaded = true;
}

Font::~Font() {
    if (face) {
        FT_Done_Face(face);
        face = nullptr;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    for (const auto& atlas : atlases) {
        glDeleteTextures(1, &atlas.texture_id);
    }
}

std::optional<Character> Font::getCharacter(Charcode charcode) {
    if (!loaded) return {};
    auto it = characters.find(charcode);
    if (it != characters.end())
        return it->second;
    else {
        if (loadGlyph(charcode)) {
            return characters.at(charcode);
        } else {
            return {};
        }
    }
}

bool Font::createNewAtlas() {
    Atlas new_atlas;

    new_atlas.width = FONT_ATLAS_DEFAULT_SIZE;
    new_atlas.height = FONT_ATLAS_DEFAULT_SIZE;
    engine_context->get_debugger()->log(DebugLevel::INFO,
                                        std::string("Creating new font atlas of size ") +
                                            std::to_string(new_atlas.width) + "x" +
                                            std::to_string(new_atlas.height),
                                        __FILE__, __LINE__);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &new_atlas.texture_id);
    glBindTexture(GL_TEXTURE_2D, new_atlas.texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, new_atlas.width, new_atlas.height, 0, GL_RED,
                 GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    if (glGetError() != GL_NO_ERROR) {
        engine_context->get_debugger()->log(
            DebugLevel::ERROR, std::string("Failed to create a new font atlas texture."), __FILE__,
            __LINE__);
        glDeleteTextures(1, &new_atlas.texture_id);
        return false;
    }

    atlases.push_back(new_atlas);
    return true;
}

bool Font::initBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return (glGetError() == GL_NO_ERROR);
}

bool Font::loadGlyph(Charcode charcode) {
    if (!face || atlases.empty()) return false;

    if (FT_Load_Char(face, charcode, FT_LOAD_RENDER)) return false;

    FT_GlyphSlot glyph = face->glyph;

    Atlas* current_atlas = nullptr;
    while (true) {
        current_atlas = &atlases.back();

        if (current_atlas->current_x + glyph->bitmap.width + 1 >= current_atlas->width) {
            current_atlas->current_y += current_atlas->max_row_height + 1;
            current_atlas->current_x = 0;
            current_atlas->max_row_height = 0;
        }

        if (current_atlas->current_y + glyph->bitmap.rows + 1 < current_atlas->height) {
            break;
        }

        if (!createNewAtlas()) {
            return false;
        }
    }

    if (glyph->bitmap.width > 0 && glyph->bitmap.rows > 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, current_atlas->texture_id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, current_atlas->current_x, current_atlas->current_y,
                        glyph->bitmap.width, glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE,
                        glyph->bitmap.buffer);
        glBindTexture(GL_TEXTURE_2D, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    Character character = {current_atlas->texture_id,
                           glm::ivec2(glyph->bitmap.width, glyph->bitmap.rows),
                           glm::ivec2(glyph->bitmap_left, glyph->bitmap_top),
                           static_cast<GLuint>(glyph->advance.x >> 6),
                           glm::vec2((float)current_atlas->current_x / current_atlas->width,
                                     (float)current_atlas->current_y / current_atlas->height),
                           glm::vec2((float)glyph->bitmap.width / current_atlas->width,
                                     (float)glyph->bitmap.rows / current_atlas->height)};
    characters.insert({charcode, character});

    current_atlas->current_x += glyph->bitmap.width + 1;
    current_atlas->max_row_height = std::max(current_atlas->max_row_height, glyph->bitmap.rows);

    return true;
}

}  // namespace vsrg