#pragma once

#include "shader.h"
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
    }

    void use() const { glUseProgram(id_); }
    static void unuse() { glUseProgram(0); }

private:
    // Save the GL ID rather than the shader object, which is more effective.
    std::vector<GLuint> attached_;
};
}
