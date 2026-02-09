#pragma once

#include <glad/glad.h>

#include "public/engineContext.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace vsrg {
struct CachedTexture {
    GLuint texture_id = 0;
    glm::ivec2 dimensions = glm::ivec2(0);
    int channels = 0;
    bool loaded = false;
    int reference_count = 0;
};

class TextureCache {
public:
    TextureCache(EngineContext* engine_context);
    ~TextureCache();

    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;

    CachedTexture* getTexture(const std::string& path);
    void addReference(const std::string& path);

    void removeReference(const std::string& path);
    void clearUnused();

    void clearAll();
    size_t getCachedTextureCount() const { return textures.size(); }

private:
    bool loadTextureFromFile(const std::string& path, CachedTexture& texture);

    EngineContext* engine_context;
    std::unordered_map<std::string, CachedTexture> textures;
};
}  // namespace vsrg