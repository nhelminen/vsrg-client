#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <glad/glad.h>

#include "public/engineContext.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace vsrg {
GLuint compileShader(EngineContext* engine_context, GLenum type, const char* source);
GLuint createShaderProgram(EngineContext* engine_context, const char* vertex_shader_source,
                           const char* fragment_shader_source);
}  // namespace vsrg