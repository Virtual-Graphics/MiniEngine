#include "base/application.h"
#include "base/layer.h"
#include "render/gl/index_buffer.h"
#include "render/gl/program.h"
#include "render/gl/shader.h"
#include "render/gl/vertex_array.h"
#include "render/gl/vertex_buffer.h"
#include "render/gl/vertex_layout.h"
#include "render/reources/mesh.h"
#include "asset/obj_loader.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>


class ClearLayer : public base::Layer {
public:
    ClearLayer()
            : base::Layer("ClearLayer") {}

    void on_attach() override {
        std::filesystem::path assets_dir = ASSETS_DIR; 
        mesh_ = std::make_unique<mini::Mesh>(mini::asset::load_obj( assets_dir / "teapot.obj" ));

        vbo_.set_data<mini::Vertex>(mesh_->vertices_);
        ibo_.set_data(mesh_->indices_, mini::gl::IndexBuffer::Type::Uint32);

        const mini::gl::VertexLayout layout{
            { 
              { 0, 3, GL_FLOAT, GL_FALSE, offsetof(mini::Vertex, position) },
              { 1, 4, GL_FLOAT, GL_FALSE, offsetof(mini::Vertex, color)    }
            },
            sizeof(mini::Vertex)
        };

        vao_.set_vertex_buffer(vbo_, layout);
        vao_.set_index_buffer(ibo_);

        view_ = glm::lookAt(glm::vec3{0.0f,0.0f,5.0f}, glm::vec3{0.0f,0.0f,0.0f}, glm::vec3{0.0f,1.0f,0.0f});

        float aspect = (float)application().get_window().width() / (float)application().get_window().height();
        proj_ = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        model_ = glm::mat4(1.0f);
        model_ = glm::translate(model_, glm::vec3{0.0f,-1.5f,0.0f});

        mvp_ = proj_ * view_ * model_;

        glEnable(GL_DEPTH_TEST);

        std::filesystem::path shaders_dir = SHADERS_DIR;

        vertex_shader_ = std::make_unique<mini::gl::Shader>(mini::gl::Shader::Type::Vertex, shaders_dir / "triangle.vert");
        fragment_shader_ = std::make_unique<mini::gl::Shader>(mini::gl::Shader::Type::Fragment, shaders_dir / "triangle.frag");
        

        program_ = std::make_unique<mini::gl::Program>();
        program_->attach(*vertex_shader_);
        program_->attach(*fragment_shader_);
        program_->link();
    }

    void on_update(base::Timestep dt) override {
        angle_ += dt * rotate_speed_;
        if (angle_ > 360.0f) {
            angle_ -= 360.0f;
        }
        model_ = glm::mat4(1.0f);
        model_ = glm::translate(model_, glm::vec3{0.0f,-1.5f,0.0f});
        model_ = glm::rotate(model_, glm::radians(angle_), glm::vec3{0.0f,1.0f,0.0f});
        mvp_ = proj_ * view_ * model_;
    }

    void on_detach() override {}

    void on_render() override {
        glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program_->use();
        
        GLint loc = glGetUniformLocation(program_->id(), "u_mvp");
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp_));

        vao_.bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vao_.index_count()), static_cast<GLenum>(vao_.index_type()), nullptr);
        mini::gl::VertexArray::unbind();
        mini::gl::Program::unuse();
    }

private:
    mini::gl::VertexBuffer vbo_;
    mini::gl::IndexBuffer ibo_;
    mini::gl::VertexArray vao_;
    std::unique_ptr<mini::Mesh> mesh_;
    float angle_ = 0;
    float rotate_speed_ = 60.0;
    glm::mat4 model_;
    glm::mat4 view_;
    glm::mat4 proj_;
    glm::mat4 mvp_;
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
