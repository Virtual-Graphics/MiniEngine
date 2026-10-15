#include "engine/application.h"
#include "engine/layer.h"
#include "gfx/command.h"
#include "gfx/context.h"
#include "gfx/index_buffer.h"
#include "gfx/pipeline_state.h"
#include "gfx/program.h"
#include "gfx/shader.h"
#include "gfx/uniform_buffer.h"
#include "gfx/vertex_array.h"
#include "gfx/vertex_buffer.h"
#include "gfx/vertex_layout.h"
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

// Must match the std140 CameraBlock layout in the shaders.
struct CameraBlock {
    glm::mat4 view_proj;
    glm::vec3 view_pos;
    float _pad = 0.0f;
};




class ClearLayer : public engine::Layer {
public:
    ClearLayer() : engine::Layer("ClearLayer") {}

    void on_attach() override {
        std::filesystem::path assets_dir = ASSETS_DIR; 
        mesh_ = std::make_unique<mini::Mesh>(mini::asset::OBJImporter::load_obj( assets_dir / "StanfordBunny.obj" ));

        vbo_.set_data<mini::Vertex>(mesh_->vertices_);
        ibo_.set_data(mesh_->indices_, mini::gfx::IndexBuffer::Type::Uint32);

        const mini::gfx::VertexLayout layout{
            { 
              { 0, 3, GL_FLOAT, GL_FALSE, offsetof(mini::Vertex, position) },
              { 1, 3, GL_FLOAT, GL_FALSE, offsetof(mini::Vertex, normal)   }
            },
            sizeof(mini::Vertex)
        };

        vao_.set_vertex_buffer(vbo_, layout);
        vao_.set_index_buffer(ibo_);

        view_pos_ = glm::vec3{0.0f,0.0f,5.0f};
        view_ = glm::lookAt(view_pos_, glm::vec3{0.0f,0.0f,0.0f}, glm::vec3{0.0f,1.0f,0.0f});

        float aspect = (float)application().get_window().width() / (float)application().get_window().height();
        proj_ = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        model_ = glm::mat4(1.0f);
        model_ = glm::scale(model_, glm::vec3(10.0f));
        model_ = glm::translate(model_, glm::vec3{0.0f,-1.0f,0.0f});

        pipeline_ = mini::gfx::PipelineState({ .depth_test = true });
        mini::gfx::Command::apply(pipeline_);

        std::filesystem::path shaders_dir = SHADERS_DIR;

        vertex_shader_ = std::make_unique<mini::gfx::Shader>(mini::gfx::Shader::Type::Vertex, shaders_dir / "blinn_phong.vert");
        fragment_shader_ = std::make_unique<mini::gfx::Shader>(mini::gfx::Shader::Type::Fragment, shaders_dir / "blinn_phong.frag");
        

        program_ = std::make_unique<mini::gfx::Program>();
        program_->attach(*vertex_shader_);
        program_->attach(*fragment_shader_);
        program_->link();
        program_->bind_uniform_block("CameraBlock", 0);

        camera_ubo_.set_data(camera_, 0);
        update_camera();
    }

    void on_update(base::Timestep dt) override {
        angle_ += dt * rotate_speed_;
        if (angle_ > 360.0f) {
            angle_ -= 360.0f;
        }
        model_ = glm::mat4(1.0f);
        model_ = glm::translate(model_, glm::vec3{0.0f,-1.0f,0.0f});
        model_ = glm::rotate(model_, glm::radians(angle_), glm::vec3{0.0f,1.0f,0.0f});
        model_ = glm::scale(model_, glm::vec3(15.0f));
    
        float aspect = static_cast<float>(application().get_window().width()) / static_cast<float>(application().get_window().height());
        proj_ = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        update_camera();
    }

    void on_detach() override {}

    void on_render() override {
        mini::gfx::Command::clear({ 0.08f, 0.09f, 0.11f, 1.0f });

        program_->use();
        program_->set("u_model", model_);
        mini::gfx::Command::draw_indexed(vao_);
        mini::gfx::Program::unuse();
    }

private:
    void update_camera() {
        camera_.view_proj = proj_ * view_;
        camera_.view_pos = view_pos_;
        camera_ubo_.set_sub_data(camera_, 0);
    }

    mini::gfx::VertexBuffer vbo_;
    mini::gfx::IndexBuffer ibo_;
    mini::gfx::VertexArray vao_;
    mini::gfx::UniformBuffer camera_ubo_;
    std::unique_ptr<mini::Mesh> mesh_;
    float angle_ = 0;
    float rotate_speed_ = 60.0;
    glm::mat4 model_;
    glm::mat4 view_;
    glm::mat4 proj_;
    glm::vec3 view_pos_;
    CameraBlock camera_;
    mini::gfx::PipelineState pipeline_;
    std::unique_ptr<mini::gfx::Shader> vertex_shader_;
    std::unique_ptr<mini::gfx::Shader> fragment_shader_;
    std::unique_ptr<mini::gfx::Program> program_;
};

int main() {
    engine::ApplicationCreateInfo info;
    info.width = 1280;
    info.height = 720;
    info.title = "MiniEngine";

    engine::Application app(info);
    app.push_layer(std::make_unique<ClearLayer>());
    app.run();
    return 0;
}
