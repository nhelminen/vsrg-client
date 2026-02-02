#pragma once	

#include "core/app.hpp"

namespace vsrg {

	GLuint compileShader(Client* client, GLenum type, const char* source);
	GLuint createShaderProgram(Client* client, const char* vertex_shader_source, const char* fragment_shader_source);

}