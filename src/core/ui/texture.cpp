#include "core/ui/texture.hpp"

#include "core/debug.hpp"
#include "core/utils.hpp"


#ifndef STB_IMAGE_IMPLEMENTATION_DONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION_DONE
#endif
#include <stb_image.h>

namespace vsrg {

TextureCache::TextureCache(EngineContext* engine_context) : engine_context(engine_context) {
    engine_context->get_debugger()->log(DebugLevel::INFO, "Texture cache initialized", __FILE__,
                                        __LINE__);
}

TextureCache::~TextureCache() {
    clearAll();
}

bool TextureCache::loadTextureFromFile(const std::string& path, CachedTexture& texture) {
    std::string execPath = getExecutableDir();
    std::string filePath = joinPaths(execPath, "assets", path);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

    if (!data) {
        engine_context->get_debugger()->log(
            DebugLevel::ERROR, std::string("Failed to load texture from path: ") + path, __FILE__,
            __LINE__);
        return false;
    }

    texture.dimensions = glm::ivec2(width, height);
    texture.channels = channels;

    glGenTextures(1, &texture.texture_id);
    glBindTexture(GL_TEXTURE_2D, texture.texture_id);

    GLenum format = GL_RGB;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    if (glGetError() != GL_NO_ERROR) {
        engine_context->get_debugger()->log(
            DebugLevel::ERROR, std::string("OpenGL error while loading texture: ") + path, __FILE__,
            __LINE__);
        glDeleteTextures(1, &texture.texture_id);
        texture.texture_id = 0;
        return false;
    }

    texture.loaded = true;
    texture.reference_count = 0;

    engine_context->get_debugger()->log(DebugLevel::INFO,
                                        std::string("Successfully cached texture: ") + path + " (" +
                                            std::to_string(width) + "x" + std::to_string(height) +
                                            ")",
                                        __FILE__, __LINE__);

    return true;
}

CachedTexture* TextureCache::getTexture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) {
        if (it->second.loaded) {
            return &it->second;
        }
    }

    CachedTexture new_texture;
    if (!loadTextureFromFile(path, new_texture)) {
        return nullptr;
    }

    auto [inserted_it, success] = textures.insert({path, new_texture});
    if (success) {
        return &inserted_it->second;
    }

    return nullptr;
}

void TextureCache::addReference(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) {
        it->second.reference_count++;
    }
}

void TextureCache::removeReference(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) {
        it->second.reference_count--;
        if (it->second.reference_count < 0) {
            it->second.reference_count = 0;
        }
    }
}

void TextureCache::clearUnused() {
    for (auto it = textures.begin(); it != textures.end();) {
        if (it->second.reference_count <= 0) {
            if (it->second.texture_id) {
                glDeleteTextures(1, &it->second.texture_id);
            }
            it = textures.erase(it);
        } else {
            ++it;
        }
    }
}

void TextureCache::clearAll() {
    for (auto& [path, texture] : textures) {
        if (texture.texture_id) {
            glDeleteTextures(1, &texture.texture_id);
        }
    }
    textures.clear();
}

}  // namespace vsrg