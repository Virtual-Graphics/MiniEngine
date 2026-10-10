#include "base/application.h"
#include "base/layer.h"
#include "render/gl/program.h"
#include "render/gl/shader.h"
#include "render/gl/vertex_buffer.h"

#include <vector>
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

namespace {

constexpr const char* kVertexShaderSource = R"(
#version 460 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;

out vec4 v_color;

void main() {
    v_color = a_color;
    gl_Position = vec4(a_position, 1.0);
}
)";

constexpr const char* kFragmentShaderSource = R"(
#version 460 core
in vec4 v_color;

out vec4 frag_color;

void main() {
    frag_color = v_color;
}
)";

} // namespace

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
};

class ClearLayer : public base::Layer {
public:
    ClearLayer() : base::Layer("ClearLayer") {}

    void on_attach() override {
        const std::vector<Vertex> vertices{
            Vertex{{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
            Vertex{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
            Vertex{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
        };
        const std::vector<uint32_t> indices{0, 1, 2};

        glCreateVertexArrays(1, &vao_);
        vbo_.set_data<Vertex>(vertices);
        glCreateBuffers(1, &ibo_);

        glNamedBufferStorage(ibo_, sizeof(indices), indices.data(), GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayVertexBuffer(vao_, 0, vbo_.id(), 0, sizeof(Vertex));
        glVertexArrayElementBuffer(vao_, ibo_);

        glEnableVertexArrayAttrib(vao_, 0);
        glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
        glVertexArrayAttribBinding(vao_, 0, 0);

        glEnableVertexArrayAttrib(vao_, 1);
        glVertexArrayAttribFormat(vao_, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
        glVertexArrayAttribBinding(vao_, 1, 0);

        vertex_shader_ = std::make_unique<mini::gl::Shader>(
            mini::gl::Shader::Type::Vertex, kVertexShaderSource);
        fragment_shader_ = std::make_unique<mini::gl::Shader>(
            mini::gl::Shader::Type::Fragment, kFragmentShaderSource);

        program_ = std::make_unique<mini::gl::Program>();
        program_->attach(*vertex_shader_);
        program_->attach(*fragment_shader_);
        program_->link();
    }

    void on_detach() override {
        glDeleteBuffers(1, &ibo_);
        glDeleteVertexArrays(1, &vao_);
    }

    void on_render() override {
        glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program_->use();
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        mini::gl::Program::unuse();
    }

private:
    mini::gl::VertexBuffer vbo_;
    GLuint vao_{0}, ibo_{0};
    std::unique_ptr<mini::gl::Shader> vertex_shader_;
    std::unique_ptr<mini::gl::Shader> fragment_shader_;
    std::unique_ptr<mini::gl::Program> program_;
};

int main() {
    base::ApplicationCreateInfo info;
    info.width = 1280;
    info.height = 720;
    info.title = "MiniEngine";

    base::Application app(info);
    app.push_layer(std::make_unique<ClearLayer>());
    app.run();
    return 0;
}
