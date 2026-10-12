#include "base/application.h"
#include "base/layer.h"
#include "render/gl/index_buffer.h"
#include "render/gl/program.h"
#include "render/gl/shader.h"
#include "render/gl/vertex_array.h"
#include "render/gl/vertex_buffer.h"
#include "render/gl/vertex_layout.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
};

class ClearLayer : public base::Layer {
public:
    ClearLayer()
            : base::Layer("ClearLayer") {}

    void on_attach() override {
        const std::vector<Vertex> vertices{
            Vertex{ { 0.0f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            Vertex{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            Vertex{ { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        };
        const std::vector<uint32_t> indices{ 0, 1, 2 };

        vbo_.set_data<Vertex>(vertices);
        ibo_.set_data(indices, mini::gl::IndexBuffer::Type::Uint32);

        const mini::gl::VertexLayout layout{
            { 
              { 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position) },
              { 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color)    }
            },
            sizeof(Vertex)
        };

        vao_.set_vertex_buffer(vbo_, layout);
        vao_.set_index_buffer(ibo_);

        std::filesystem::path shaders_dir = SHADERS_DIR;

        vertex_shader_ = std::make_unique<mini::gl::Shader>(mini::gl::Shader::Type::Vertex, shaders_dir / "triangle.vert");
        fragment_shader_ = std::make_unique<mini::gl::Shader>(mini::gl::Shader::Type::Fragment, shaders_dir / "triangle.frag");
        

        program_ = std::make_unique<mini::gl::Program>();
        program_->attach(*vertex_shader_);
        program_->attach(*fragment_shader_);
        program_->link();
    }

    void on_detach() override {}

    void on_render() override {
        glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program_->use();
        vao_.bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vao_.index_count()), static_cast<GLenum>(vao_.index_type()), nullptr);
        mini::gl::VertexArray::unbind();
        mini::gl::Program::unuse();
    }

private:
    mini::gl::VertexBuffer vbo_;
    mini::gl::IndexBuffer ibo_;
    mini::gl::VertexArray vao_;
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
