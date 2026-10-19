#pragma once
#include "base/logger.h"
#include "object.h"
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace vulcal::gfx{
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

    Shader(Type type, const std::filesystem::path& path) : Shader(type,read_file(path.string())) {
        base::log::debug("Compiled {} shader: {}", type_name(type), path.string());
    }

    Shader(Type type, const char* source) : Shader(type,std::string(source)) {}

private:

    void compile(Type type, std::string_view source){
        reset();

        id_ = glCreateShader(static_cast<GLenum>(type));

        const char* src = source.data();
        GLint len = static_cast<GLint>(source.size());
        glShaderSource(id_, 1, &src, &len);
        glCompileShader(id_);

        GLint ok = GL_FALSE;
        glGetShaderiv(id_, GL_COMPILE_STATUS, &ok);
        if (ok == GL_FALSE) {
            const std::string log = info_log(id_);
            base::log::error("Shader compile failed:\n{}", log);
            throw std::runtime_error("Shader compilation failed");
        }
    }

    static std::string info_log(GLuint id) {
        GLint len = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        if (len <= 1) {
            return {};
        }
        std::string log(static_cast<std::size_t>(len), '\0');
        GLsizei written = 0;
        glGetShaderInfoLog(id, len, &written, log.data());
        log.resize(static_cast<std::size_t>(written));
        return log;
    }

    static const char* type_name(Type type) {
        switch (type) {
        case Type::Vertex:   return "vertex";
        case Type::Fragment: return "fragment";
        case Type::Compute:  return "compute";
        }
        return "unknown";
    }

    static std::string read_file(const std::string& path){
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            base::log::error("Could not open shader file: {}", path);
            throw std::runtime_error("Cant open shader file :" + path);
        }
        std::ostringstream oss;
        oss << file.rdbuf();
        return oss.str();
    }
};
}

