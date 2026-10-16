#pragma once
#include "gfx/pipeline_state.h"
#include "gfx/program.h"
#include "gfx/uniform_buffer.h"
#include "render/renderer_data.h"
#include "render/resources/material.h"

namespace mini::gfx {
class RenderTarget;
}
namespace mini::render {

class MeshGPU;
class Pipeline {
public:
    Pipeline();
    void prepare(const RenderData& data, gfx::RenderTarget* render_target);
    void draw(const MeshGPU& mesh, const Material& material, const glm::mat4& world_transform);
    void destroy();
private:
    gfx::Program program_;
    gfx::UniformBuffer camera_ubo_;
    gfx::PipelineState pipeline_state_;
};

}