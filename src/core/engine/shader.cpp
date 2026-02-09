#include "core/engine/shader.hpp"

#include "core/debug.hpp"


namespace vsrg {

GLuint compileShader(EngineContext* engine_context, GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        engine_context->get_debugger()->log(DebugLevel::ERROR,
                                            std::string("Shader compilation failed: ") + info_log,
                                            __FILE__, __LINE__);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createShaderProgram(EngineContext* engine_context, const char* vertex_shader_source,
                           const char* fragment_shader_source) {
    GLuint vertex_shader = compileShader(engine_context, GL_VERTEX_SHADER, vertex_shader_source);
    if (!vertex_shader) return 0;

    GLuint fragment_shader =
        compileShader(engine_context, GL_FRAGMENT_SHADER, fragment_shader_source);
    if (!fragment_shader) {
        glDeleteShader(vertex_shader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar info_log[512];
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        engine_context->get_debugger()->log(
            DebugLevel::ERROR, std::string("Shader program linking failed: ") + info_log, __FILE__,
            __LINE__);
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

}  // namespace vsrg