#pragma once
#include "gfx/pipeline_state.h"
#include "gfx/vertex_array.h"
#include <cstdint>
#include <glm/glm.hpp>

namespace mini::gfx {

enum class Primitive {
    Points,
    Lines,
    LineStrip,
    LineLoop,
    Triangles,
    TriangleStrip,
    TriangleFan,
};

class Command {
public:
    Command() = delete;

    static void set_viewport(std::uint32_t width, std::uint32_t height);

    static void clear(const glm::vec4& color, float depth = 1.0f);
    static void clear_color(const glm::vec4& color);
    static void clear_depth(float depth = 1.0f);

    static void apply(const PipelineState& pipeline);

    static void draw_indexed(const VertexArray& vao, Primitive primitive = Primitive::Triangles);
};

} // namespace mini::gfx
