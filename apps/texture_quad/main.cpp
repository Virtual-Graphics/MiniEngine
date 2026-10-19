#include "asset/image_loader.h"
#include "engine/application.h"
#include "engine/layer.h"
#include "gfx/command.h"
#include "gfx/image.h"
#include "gfx/index_buffer.h"
#include "gfx/program.h"
#include "gfx/sampler.h"
#include "gfx/shader.h"
#include "gfx/texture.h"
#include "gfx/vertex_array.h"
#include "gfx/vertex_buffer.h"
#include "gfx/vertex_layout.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace {

struct QuadVertex {
    glm::vec2 position;
    glm::vec2 uv;
};

}

class TextureQuadLayer : public engine::Layer {
public:
    TextureQuadLayer() : engine::Layer("TextureQuadLayer") {}

    void on_attach() override {
        const std::vector<QuadVertex> vertices{
            { {-0.75f, -0.75f}, {0.0f, 0.0f} },
            { { 0.75f, -0.75f}, {1.0f, 0.0f} },
            { { 0.75f,  0.75f}, {1.0f, 1.0f} },
            { {-0.75f,  0.75f}, {0.0f, 1.0f} },
        };
        const std::vector<std::uint32_t> indices{ 0, 1, 2, 2, 3, 0 };

        vbo_.set_data(vertices);
        ibo_.set_data(indices, vulcal::gfx::IndexBuffer::Type::Uint32);

        const vulcal::gfx::VertexLayout layout{
            {
                { 0, 2, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, position) },
                { 1, 2, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, uv) },
            },
            sizeof(QuadVertex)
        };

        vao_.set_vertex_buffer(vbo_, layout);
        vao_.set_index_buffer(ibo_);

        std::filesystem::path assets_dir = ASSETS_DIR;
        const vulcal::asset::ImageData data = vulcal::asset::load_image(assets_dir / "artichoke.jpg", 4);

        vulcal::gfx::Image image(vulcal::gfx::Image::Format::RGBA8, data.width, data.height,
                               vulcal::gfx::Image::mip_count(data.width, data.height));
        image.set_data(data.pixels.data());
        image.generate_mipmaps();

        vulcal::gfx::Sampler::Spec spec = vulcal::gfx::Sampler::Spec::clamp_to_edge();
        spec.min_filter = vulcal::gfx::Sampler::Filter::LinearMipmapLinear;

        texture_ = std::make_unique<vulcal::gfx::Texture>(std::move(image), vulcal::gfx::Sampler(spec));

        std::filesystem::path shaders_dir = SHADERS_DIR;

        vertex_shader_ = std::make_unique<vulcal::gfx::Shader>(vulcal::gfx::Shader::Type::Vertex, shaders_dir / "texture.vert");
        fragment_shader_ = std::make_unique<vulcal::gfx::Shader>(vulcal::gfx::Shader::Type::Fragment, shaders_dir / "texture.frag");

        program_ = std::make_unique<vulcal::gfx::Program>();
        program_->attach(*vertex_shader_);
        program_->attach(*fragment_shader_);
        program_->link();
    }

    void on_update(base::Timestep) override {}

    void on_detach() override {}

    void on_render() override {
        vulcal::gfx::Command::clear({ 0.08f, 0.09f, 0.11f, 1.0f });

        program_->use();
        program_->set("u_texture", 0);
        texture_->bind(0);
        vulcal::gfx::Command::draw_indexed(vao_);
        vulcal::gfx::Program::unuse();
    }

private:
    vulcal::gfx::VertexBuffer vbo_;
    vulcal::gfx::IndexBuffer ibo_;
    vulcal::gfx::VertexArray vao_;
    std::unique_ptr<vulcal::gfx::Texture> texture_;
    std::unique_ptr<vulcal::gfx::Shader> vertex_shader_;
    std::unique_ptr<vulcal::gfx::Shader> fragment_shader_;
    std::unique_ptr<vulcal::gfx::Program> program_;
};

int main() {
    engine::ApplicationCreateInfo info;
    info.width = 1280;
    info.height = 720;
    info.title = "VulcalEngine - Texture Quad Test";

    engine::Application app(info);
    app.push_layer(std::make_unique<TextureQuadLayer>());
    app.run();
    return 0;
}
