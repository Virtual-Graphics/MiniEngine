#pragma once

#include "shader.h"
namespace mini::gl{
class Program : public Object {
public:
    Program() : Object([](GLuint id){ glDeleteProgram(id); }) {
        id_ = glCreateProgram();
    }

    void attach(const Shader& s) {
        glAttachShader(id_, s.id());
        m_attached.push_back(s.id());
    }

    void link() {
        glLinkProgram(id_);

        for (GLuint sid : m_attached) 
            glDetachShader(id_, sid);
        m_attached.clear();
    }

    void use() const { glUseProgram(id_); }
    static void unuse() { glUseProgram(0); }

private:
    // Save the GL ID rather than the shader object, which is more effective.
    std::vector<GLuint> m_attached;
};
}
