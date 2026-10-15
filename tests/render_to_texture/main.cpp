#include "asset/obj_loader.h"
#include "engine/application.h"
#include "engine/layer.h"
#include "gfx/index_buffer.h"
#include "gfx/program.h"
#include "gfx/render_target.h"
#include "gfx/shader.h"
#include "gfx/uniform_buffer.h"
#include "gfx/vertex_array.h"
#include "gfx/vertex_buffer.h"
#include "gfx/vertex_layout.h"
#include "render/reources/mesh.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

struct CameraBlock {
    glm::mat4 view_proj;
    glm::vec3 view_pos;
    float _pad = 0.0f;
};

namespace {

struct QuadVertex {
    glm::vec2 position;
    glm::vec2 uv;
};

} // namespace

class RenderToTextureLayer : public engine::Layer {
public:
    RenderToTextureLayer() : engine::Layer("RenderToTextureLayer") {}

    void on_attach() override {
        auto& window = application().get_window();

        mini::gfx::RenderTargetSpec target_spec;
        target_spec.width = window.width();
        target_spec.height = window.height();
        target_spec.color_format = mini::gfx::Image::Format::RGBA8;
        target_spec.with_depth = true;
        target_spec.depth_format = mini::gfx::Image::Format::Depth24Stencil8;
        render_target_ = std::make_unique<mini::gfx::RenderTarget>(target_spec);

        std::filesystem::path assets_dir = ASSETS_DIR;
        mesh_ = std::make_unique<mini::Mesh>(mini::asset::OBJImporter::load_obj(assets_dir / "StanfordBunny.obj"));

        vbo_.set_data(mesh_->vertices_);
        ibo_.set_data(mesh_->indices_, mini::gfx::IndexBuffer::Type::Uint32);

        const mini::gfx::VertexLayout mesh_layout{
            { { 0, 3, GL_FLOAT, GL_FALSE, offsetof(mini::Vertex, position) },
                { 1, 3, GL_FLOAT, GL_FALSE, offsetof(mini::Vertex, normal) } },
            sizeof(mini::Vertex)
        };

        mesh_vao_.set_vertex_buffer(vbo_, mesh_layout);
        mesh_vao_.set_index_buffer(ibo_);

        const std::vector<QuadVertex> quad_vertices{
            { { -1.0f, -1.0f }, { 0.0f, 0.0f } },
            { { 1.0f, -1.0f }, { 1.0f, 0.0f } },
            { { 1.0f, 1.0f }, { 1.0f, 1.0f } },
            { { -1.0f, 1.0f }, { 0.0f, 1.0f } },
        };
        const std::vector<std::uint32_t> quad_indices{ 0, 1, 2, 2, 3, 0 };

        quad_vbo_.set_data(quad_vertices);
        quad_ibo_.set_data(quad_indices, mini::gfx::IndexBuffer::Type::Uint32);

        const mini::gfx::VertexLayout quad_layout{
            {
                { 0, 2, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, position) },
                { 1, 2, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, uv) },
            },
            sizeof(QuadVertex)
        };

        quad_vao_.set_vertex_buffer(quad_vbo_, quad_layout);
        quad_vao_.set_index_buffer(quad_ibo_);

        view_pos_ = glm::vec3{ 0.0f, 0.0f, 5.0f };
        view_ = glm::lookAt(view_pos_, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f });

        model_ = glm::mat4(1.0f);
        model_ = glm::scale(model_, glm::vec3(10.0f));
        model_ = glm::translate(model_, glm::vec3{ 0.0f, -1.0f, 0.0f });

        glEnable(GL_DEPTH_TEST);

        std::filesystem::path shaders_dir = SHADERS_DIR;

        vertex_shader_ = std::make_unique<mini::gfx::Shader>(mini::gfx::Shader::Type::Vertex, shaders_dir / "blinn_phong.vert");
        fragment_shader_ = std::make_unique<mini::gfx::Shader>(mini::gfx::Shader::Type::Fragment, shaders_dir / "blinn_phong.frag");

        program_ = std::make_unique<mini::gfx::Program>();
        program_->attach(*vertex_shader_);
        program_->attach(*fragment_shader_);
        program_->link();
        program_->bind_uniform_block("CameraBlock", 0);

        screen_vertex_shader_ = std::make_unique<mini::gfx::Shader>(mini::gfx::Shader::Type::Vertex, shaders_dir / "screen.vert");
        screen_fragment_shader_ = std::make_unique<mini::gfx::Shader>(mini::gfx::Shader::Type::Fragment, shaders_dir / "screen.frag");

        screen_program_ = std::make_unique<mini::gfx::Program>();
        screen_program_->attach(*screen_vertex_shader_);
        screen_program_->attach(*screen_fragment_shader_);
        screen_program_->link();

        update_camera();
    }

    void on_update(base::Timestep dt) override {
        angle_ += dt * rotate_speed_;
        if (angle_ > 360.0f) {
            angle_ -= 360.0f;
        }
        model_ = glm::mat4(1.0f);
        model_ = glm::translate(model_, glm::vec3{ 0.0f, -1.0f, 0.0f });
        model_ = glm::rotate(model_, glm::radians(angle_), glm::vec3{ 0.0f, 1.0f, 0.0f });
        model_ = glm::scale(model_, glm::vec3(15.0f));

        auto& window = application().get_window();
        if (window.resized()) {
            render_target_->resize(window.width(), window.height());
            window.clear_resized();
        }

        const float aspect = static_cast<float>(window.width()) / static_cast<float>(window.height());
        proj_ = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        update_camera();
    }

    void on_detach() override {}

    void on_render() override {
        render_target_->bind();
        render_target_->clear(clear_color_, 1.0f);

        program_->use();
        program_->set("u_model", model_);
        mesh_vao_.bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh_vao_.index_count()), static_cast<GLenum>(mesh_vao_.index_type()), nullptr);

        mini::gfx::RenderTarget::unbind();

        auto& window = application().get_window();
        glViewport(0, 0, static_cast<GLsizei>(window.width()), static_cast<GLsizei>(window.height()));

        glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, clear_color_.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        screen_program_->use();
        screen_program_->set("u_texture", 0);
        render_target_->color().bind(0);
        quad_vao_.bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(quad_vao_.index_count()), static_cast<GLenum>(quad_vao_.index_type()), nullptr);
        mini::gfx::VertexArray::unbind();
        mini::gfx::Program::unuse();
    }

private:
    void update_camera() {
        camera_.view_proj = proj_ * view_;
        camera_.view_pos = view_pos_;
        camera_ubo_.set_data(camera_, 0);
    }

    std::unique_ptr<mini::gfx::RenderTarget> render_target_;
    mini::gfx::VertexBuffer vbo_;
    mini::gfx::IndexBuffer ibo_;
    mini::gfx::VertexArray mesh_vao_;
    mini::gfx::VertexBuffer quad_vbo_;
    mini::gfx::IndexBuffer quad_ibo_;
    mini::gfx::VertexArray quad_vao_;
    mini::gfx::UniformBuffer camera_ubo_;
    std::unique_ptr<mini::Mesh> mesh_;
    float angle_ = 0;
    float rotate_speed_ = 60.0;
    glm::mat4 model_;
    glm::mat4 view_;
    glm::mat4 proj_;
    glm::vec3 view_pos_;
    CameraBlock camera_;
    glm::vec4 clear_color_{ 0.08f, 0.09f, 0.11f, 1.0f };
    std::unique_ptr<mini::gfx::Shader> vertex_shader_;
    std::unique_ptr<mini::gfx::Shader> fragment_shader_;
    std::unique_ptr<mini::gfx::Program> program_;
    std::unique_ptr<mini::gfx::Shader> screen_vertex_shader_;
    std::unique_ptr<mini::gfx::Shader> screen_fragment_shader_;
    std::unique_ptr<mini::gfx::Program> screen_program_;
};

int main() {
    engine::ApplicationCreateInfo info;
    info.width = 1280;
    info.height = 720;
    info.title = "MiniEngine - Render To Texture Test";

    engine::Application app(info);
    app.push_layer(std::make_unique<RenderToTextureLayer>());
    app.run();
    return 0;
}
