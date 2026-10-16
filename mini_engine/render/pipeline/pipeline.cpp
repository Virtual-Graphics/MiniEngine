#include "render/pipeline/pipeline.h"

#include "gfx/command.h"
#include "gfx/program.h"
#include "gfx/render_target.h"
#include "gfx/shader.h"
#include "gfx/vertex_array.h"
#include "render/resources/mesh_gpu.h"
#include <filesystem>
namespace mini::render {
namespace {
    struct alignas(16) CameraUBO {
        glm::mat4 view_proj_;
        glm::vec4 position_;
    };
}


Pipeline::Pipeline() : pipeline_state_({.depth_test = true}) {
    const std::filesystem::path shaders_dir = MINI_RENDER_SHADER_DIR;
    gfx::Shader vertex_shader(gfx::Shader::Type::Vertex, shaders_dir / "blinn_phong.vert");
    gfx::Shader fragment_shader(gfx::Shader::Type::Fragment, shaders_dir / "blinn_phong.frag");
    program_.attach(vertex_shader);
    program_.attach(fragment_shader);
    program_.link();
    program_.bind_uniform_block("CameraBlock", 0);
    camera_ubo_.set_data(CameraUBO{glm::mat4(1.0f), glm::vec4()}, 0);
}

void Pipeline::prepare(const RenderData& data, gfx::RenderTarget* render_target) {
    if (render_target != nullptr) {
        render_target->bind();
    }
    gfx::PipelineState::reset_cache();
    gfx::Command::apply(pipeline_state_);
    if (render_target != nullptr) {
        render_target->clear(data.environment_.clear_color_, 1.0f);
    } else {
        gfx::Command::clear(data.environment_.clear_color_);
    }

    const CameraUBO camera_ubo{data.camera_.projection_ * data.camera_.view_, glm::vec4(data.camera_.position_, 1.0f)};
    camera_ubo_.set_sub_data(camera_ubo);
    camera_ubo_.bind_base(0);
    program_.use();
    program_.set("u_ambient_color", data.environment_.ambient_color_);
    if (data.directional_light_) {
        const auto& light = *data.directional_light_;
        program_.set("u_light_direction", glm::normalize(light.direction_));
        program_.set("u_light_color", light.color_ * light.intensity_);
    } else {
        program_.set("u_light_direction", glm::vec3(0.0f, -1.0f, 0.0f));
        program_.set("u_light_color", glm::vec3(0.0f));
    }
}

void Pipeline::draw(const MeshGPU& mesh, const Material& material, const glm::mat4& world_transform) {
    program_.set("u_model", world_transform);
    program_.set("u_normal_matrix", glm::transpose(glm::inverse(glm::mat3(world_transform))));
    program_.set("u_diffuse", material.diffuse_);
    program_.set("u_specular", material.specular_);
    program_.set("u_shininess", material.shininess_);
    gfx::Command::draw_indexed(mesh.vertex_array());
}

void Pipeline::destroy(){
    gfx::Program::unuse();
    gfx::VertexArray::unbind();
    gfx::RenderTarget::unbind();
}
}
