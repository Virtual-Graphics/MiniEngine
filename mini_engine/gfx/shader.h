#pragma once
#include "object.h"
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace mini::gfx{
class Shader : public Object{
public:
    enum class Type {
        Vertex         = GL_VERTEX_SHADER,
        Fragment       = GL_FRAGMENT_SHADER,
        Compute        = GL_COMPUTE_SHADER,
    };

    Shader(Type type, const std::string& source) : Object([](GLuint id){ glDeleteShader(id);}){
        compile(type, source);
    }

    Shader(Type type, const std::filesystem::path& path) : Shader(type,read_file(path)) {}

    Shader(Type type, const char* source) : Shader(type,std::string(source)) {}

private:

    void compile(Type type, std::string_view source){
        reset();

        id_ = glCreateShader(static_cast<GLenum>(type));

        const char* src = source.data();
        GLint len = static_cast<GLint>(source.size());
        glShaderSource(id_, 1, &src, &len);
        glCompileShader(id_);
    }

    static std::string read_file(const std::string& path){
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Cant open shader file :" + path);
        std::ostringstream oss;
        oss << file.rdbuf();
        return oss.str();
    }
};
}

