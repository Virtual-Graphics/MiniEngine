#include "render/pipeline/pass.h"

#include "gfx/command.h"
#include "gfx/shader.h"
#include <filesystem>

namespace vulcal::render {

Pass::Pass(const gfx::PipelineState::Spec& pipeline_spec) : pipeline_state_(pipeline_spec) {}

void Pass::load_shaders(const char* vertex_file, const char* fragment_file) {
    const std::filesystem::path shaders_dir = VULCAL_RENDER_SHADER_DIR;
    gfx::Shader vertex_shader(gfx::Shader::Type::Vertex, shaders_dir / vertex_file);
    gfx::Shader fragment_shader(gfx::Shader::Type::Fragment, shaders_dir / fragment_file);
    program_.attach(vertex_shader);
    program_.attach(fragment_shader);
    program_.link();
}

void Pass::bind_pipeline() {
    gfx::PipelineState::reset_cache();
    gfx::Command::apply(pipeline_state_);
    program_.use();
}

} // namespace vulcal::render
