#pragma once

#include "base/logger.h"
#include "shader.h"
#include <stdexcept>
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

    // Save the GL ID rather than the shader object, which is more effective.
    std::vector<GLuint> attached_;
};
}
