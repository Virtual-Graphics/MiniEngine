#pragma once

#include "base/logger.h"
#include "shader.h"
#include <stdexcept>
#include <cstddef>
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
        base::log::debug("Linked program {}", id_);
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
    void set(const char* name, std::uint32_t v)    { glUniform1ui(location(name), v); }

    void set(const char* name, const glm::vec2& v) { glUniform2fv(location(name), 1, glm::value_ptr(v)); }
    void set(const char* name, const glm::vec3& v) { glUniform3fv(location(name), 1, glm::value_ptr(v)); }
    void set(const char* name, const glm::vec4& v) { glUniform4fv(location(name), 1, glm::value_ptr(v)); }

    void set(const char* name, const glm::ivec2& v) { glUniform2iv(location(name), 1, glm::value_ptr(v)); }
    void set(const char* name, const glm::ivec3& v) { glUniform3iv(location(name), 1, glm::value_ptr(v)); }
    void set(const char* name, const glm::ivec4& v) { glUniform4iv(location(name), 1, glm::value_ptr(v)); }

    void set(const char* name, const glm::uvec2& v) { glUniform2uiv(location(name), 1, glm::value_ptr(v)); }
    void set(const char* name, const glm::uvec3& v) { glUniform3uiv(location(name), 1, glm::value_ptr(v)); }
    void set(const char* name, const glm::uvec4& v) { glUniform4uiv(location(name), 1, glm::value_ptr(v)); }

    void set(const char* name, const glm::mat3& v) { glUniformMatrix3fv(location(name), 1, GL_FALSE, glm::value_ptr(v)); }
    void set(const char* name, const glm::mat4& v) { glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(v)); }

    void set(const char* name, const float* v, std::size_t count)          { glUniform1fv(location(name), static_cast<GLsizei>(count), v); }
    void set(const char* name, const glm::vec2* v, std::size_t count)      { glUniform2fv(location(name), static_cast<GLsizei>(count), glm::value_ptr(*v)); }
    void set(const char* name, const glm::vec3* v, std::size_t count)      { glUniform3fv(location(name), static_cast<GLsizei>(count), glm::value_ptr(*v)); }
    void set(const char* name, const glm::vec4* v, std::size_t count)      { glUniform4fv(location(name), static_cast<GLsizei>(count), glm::value_ptr(*v)); }

    void set(const char* name, const int* v, std::size_t count)            { glUniform1iv(location(name), static_cast<GLsizei>(count), v); }
    void set(const char* name, const glm::ivec2* v, std::size_t count)     { glUniform2iv(location(name), static_cast<GLsizei>(count), glm::value_ptr(*v)); }
    void set(const char* name, const glm::ivec3* v, std::size_t count)     { glUniform3iv(location(name), static_cast<GLsizei>(count), glm::value_ptr(*v)); }
    void set(const char* name, const glm::ivec4* v, std::size_t count)     { glUniform4iv(location(name), static_cast<GLsizei>(count), glm::value_ptr(*v)); }

    void set(const char* name, const std::uint32_t* v, std::size_t count)  { glUniform1uiv(location(name), static_cast<GLsizei>(count), v); }
    void set(const char* name, const glm::uvec2* v, std::size_t count)     { glUniform2uiv(location(name), static_cast<GLsizei>(count), glm::value_ptr(*v)); }
    void set(const char* name, const glm::uvec3* v, std::size_t count)     { glUniform3uiv(location(name), static_cast<GLsizei>(count), glm::value_ptr(*v)); }
    void set(const char* name, const glm::uvec4* v, std::size_t count)     { glUniform4uiv(location(name), static_cast<GLsizei>(count), glm::value_ptr(*v)); }

    void set(const char* name, const glm::mat3* v, std::size_t count)      { glUniformMatrix3fv(location(name), static_cast<GLsizei>(count), GL_FALSE, glm::value_ptr(*v)); }
    void set(const char* name, const glm::mat4* v, std::size_t count)      { glUniformMatrix4fv(location(name), static_cast<GLsizei>(count), GL_FALSE, glm::value_ptr(*v)); }
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
