#include "gfx/pipeline_state.h"

namespace mini::gfx {

namespace {

struct StateCache {
    bool valid = true;

    bool depth_test = false;
    bool depth_write = true;
    GLenum depth_func = GL_LESS;

    bool cull = false;
    GLenum cull_face = GL_BACK;
    GLenum front_face = GL_CCW;

    bool blend = false;
    GLenum src_rgb = GL_ONE;
    GLenum dst_rgb = GL_ZERO;
    GLenum src_alpha = GL_ONE;
    GLenum dst_alpha = GL_ZERO;
    GLenum blend_eq_rgb = GL_FUNC_ADD;
    GLenum blend_eq_alpha = GL_FUNC_ADD;

    glm::bvec4 color_write{ true, true, true, true };
};

StateCache g_cache;

} // namespace

GLenum PipelineState::to_gl(CullFace face) {
    switch (face) {
    case CullFace::Front: return GL_FRONT;
    case CullFace::Back:  return GL_BACK;
    }
    return GL_BACK;
}

GLenum PipelineState::to_gl(FrontFace face) {
    switch (face) {
    case FrontFace::CCW: return GL_CCW;
    case FrontFace::CW:  return GL_CW;
    }
    return GL_CCW;
}

GLenum PipelineState::to_gl(BlendFactor factor) {
    switch (factor) {
    case BlendFactor::Zero:                return GL_ZERO;
    case BlendFactor::One:                 return GL_ONE;
    case BlendFactor::SrcColor:            return GL_SRC_COLOR;
    case BlendFactor::OneMinusSrcColor:    return GL_ONE_MINUS_SRC_COLOR;
    case BlendFactor::DstColor:            return GL_DST_COLOR;
    case BlendFactor::OneMinusDstColor:    return GL_ONE_MINUS_DST_COLOR;
    case BlendFactor::SrcAlpha:            return GL_SRC_ALPHA;
    case BlendFactor::OneMinusSrcAlpha:    return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DstAlpha:            return GL_DST_ALPHA;
    case BlendFactor::OneMinusDstAlpha:    return GL_ONE_MINUS_DST_ALPHA;
    case BlendFactor::SrcAlphaSaturate:    return GL_SRC_ALPHA_SATURATE;
    }
    return GL_ONE;
}

GLenum PipelineState::to_gl(BlendEquation equation) {
    switch (equation) {
    case BlendEquation::Add:             return GL_FUNC_ADD;
    case BlendEquation::Subtract:        return GL_FUNC_SUBTRACT;
    case BlendEquation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
    case BlendEquation::Min:             return GL_MIN;
    case BlendEquation::Max:             return GL_MAX;
    }
    return GL_FUNC_ADD;
}

PipelineState::PipelineState(const Spec& spec) : spec_(spec) {}

void PipelineState::apply() const {
    const bool full = !g_cache.valid;

    if (full || g_cache.depth_test != spec_.depth_test) {
        if (spec_.depth_test) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
        g_cache.depth_test = spec_.depth_test;
    }

    if (full || g_cache.depth_write != spec_.depth_write) {
        glDepthMask(spec_.depth_write ? GL_TRUE : GL_FALSE);
        g_cache.depth_write = spec_.depth_write;
    }

    const GLenum depth_func = mini::gfx::to_gl(spec_.depth_func);
    if (full || g_cache.depth_func != depth_func) {
        glDepthFunc(depth_func);
        g_cache.depth_func = depth_func;
    }

    if (full || g_cache.cull != spec_.cull) {
        if (spec_.cull) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }
        g_cache.cull = spec_.cull;
    }

    const GLenum cull_face = to_gl(spec_.cull_face);
    if (full || g_cache.cull_face != cull_face) {
        glCullFace(cull_face);
        g_cache.cull_face = cull_face;
    }

    const GLenum front_face = to_gl(spec_.front_face);
    if (full || g_cache.front_face != front_face) {
        glFrontFace(front_face);
        g_cache.front_face = front_face;
    }

    if (full || g_cache.blend != spec_.blend) {
        if (spec_.blend) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
        g_cache.blend = spec_.blend;
    }

    const GLenum src_rgb = to_gl(spec_.src_rgb);
    const GLenum dst_rgb = to_gl(spec_.dst_rgb);
    const GLenum src_alpha = to_gl(spec_.src_alpha);
    const GLenum dst_alpha = to_gl(spec_.dst_alpha);
    if (full || g_cache.src_rgb != src_rgb || g_cache.dst_rgb != dst_rgb ||
            g_cache.src_alpha != src_alpha || g_cache.dst_alpha != dst_alpha) {
        glBlendFuncSeparate(src_rgb, dst_rgb, src_alpha, dst_alpha);
        g_cache.src_rgb = src_rgb;
        g_cache.dst_rgb = dst_rgb;
        g_cache.src_alpha = src_alpha;
        g_cache.dst_alpha = dst_alpha;
    }

    const GLenum blend_eq_rgb = to_gl(spec_.blend_eq_rgb);
    const GLenum blend_eq_alpha = to_gl(spec_.blend_eq_alpha);
    if (full || g_cache.blend_eq_rgb != blend_eq_rgb || g_cache.blend_eq_alpha != blend_eq_alpha) {
        glBlendEquationSeparate(blend_eq_rgb, blend_eq_alpha);
        g_cache.blend_eq_rgb = blend_eq_rgb;
        g_cache.blend_eq_alpha = blend_eq_alpha;
    }

    if (full || g_cache.color_write != spec_.color_write) {
        glColorMask(spec_.color_write.r ? GL_TRUE : GL_FALSE,
                    spec_.color_write.g ? GL_TRUE : GL_FALSE,
                    spec_.color_write.b ? GL_TRUE : GL_FALSE,
                    spec_.color_write.a ? GL_TRUE : GL_FALSE);
        g_cache.color_write = spec_.color_write;
    }

    g_cache.valid = true;
}

void PipelineState::reset_cache() {
    g_cache.valid = false;
}

} // namespace mini::gfx
