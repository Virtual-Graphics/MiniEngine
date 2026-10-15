#pragma once

#include "base/logger.h"
#include "shader.h"
#include <stdexcept>
#include <cstdint>
#include <glm/glm.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <string>
namespace mini::gfx{
class Program : public Object {
public:
    Program() : Object([](GLuint id){ glDeleteProgram(id); }) {
        id_ = glCreateProgram();
    }

    void attach(const Shader& s) {
        glAttachShader(id_, s.id());
        attached_.push_back(s.id());
    }

    void link() {
        glLinkProgram(id_);

        for (GLuint sid : attached_) 
            glDetachShader(id_, sid);
        attached_.clear();

        GLint ok = GL_FALSE;
        glGetProgramiv(id_, GL_LINK_STATUS, &ok);
        if (ok == GL_FALSE) {
            const std::string log = info_log(id_);
            base::log::error("Program link failed:\n{}", log);
            throw std::runtime_error("Program link failed");
        }
    }

    void use() const { glUseProgram(id_); }
    static void unuse() { glUseProgram(0); }

    void bind_uniform_block(const char* name, std::uint32_t binding) {
        const GLuint index = glGetUniformBlockIndex(id_, name);
        if (index == GL_INVALID_INDEX) {
            base::log::error("uniform block not found: {}", name);
            return;
        }
        glUniformBlockBinding(id_, index, binding);
    }

    void set(const char* name, float v)            { glUniform1f(location(name), v); }
    void set(const char* name, int v)              { glUniform1i(location(name), v); }

    void set(const char* name, const glm::vec2& v) { glUniform2fv(location(name), 1, glm::value_ptr(v)); }
    void set(const char* name, const glm::vec3& v) { glUniform3fv(location(name), 1, glm::value_ptr(v)); }
    void set(const char* name, const glm::vec4& v) { glUniform4fv(location(name), 1, glm::value_ptr(v)); }

    void set(const char* name, const glm::mat3& v) { glUniformMatrix3fv(location(name), 1, GL_FALSE, glm::value_ptr(v)); }
    void set(const char* name, const glm::mat4& v) { glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(v)); }
private:
    static std::string info_log(GLuint id) {
        GLint len = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
        if (len <= 1) {
            return {};
        }
        std::string log(static_cast<std::size_t>(len), '\0');
        GLsizei written = 0;
        glGetProgramInfoLog(id, len, &written, log.data());
        log.resize(static_cast<std::size_t>(written));
        return log;
    }

    GLint location(const char* name) {
        auto it = locations_.find(name);
        if (it != locations_.end()) return it->second;
        GLint loc = glGetUniformLocation(id_, name);
        locations_.emplace(name, loc);
        return loc;
    }
    std::unordered_map<std::string, GLint> locations_;
    // Save the GL ID rather than the shader object, which is more effective.
    std::vector<GLuint> attached_;
};
}
