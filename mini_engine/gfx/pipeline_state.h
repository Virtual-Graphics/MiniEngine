#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace mini::gfx {

class PipelineState {
public:
    enum class DepthFunc {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always,
    };

    enum class CullFace {
        Front,
        Back,
    };

    enum class FrontFace {
        CCW,
        CW,
    };

    enum class BlendFactor {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        SrcAlphaSaturate,
    };

    enum class BlendEquation {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max,
    };

    struct Spec {
        bool depth_test = false;
        bool depth_write = true;
        DepthFunc depth_func = DepthFunc::Less;

        bool cull = false;
        CullFace cull_face = CullFace::Back;
        FrontFace front_face = FrontFace::CCW;

        bool blend = false;
        BlendFactor src_rgb = BlendFactor::One;
        BlendFactor dst_rgb = BlendFactor::Zero;
        BlendFactor src_alpha = BlendFactor::One;
        BlendFactor dst_alpha = BlendFactor::Zero;
        BlendEquation blend_eq_rgb = BlendEquation::Add;
        BlendEquation blend_eq_alpha = BlendEquation::Add;

        glm::bvec4 color_write{ true, true, true, true };
    };

    PipelineState() = default;
    explicit PipelineState(const Spec& spec);

    void apply() const;

    const Spec& spec() const { return spec_; }

    static void reset_cache();

private:
    static GLenum to_gl(DepthFunc func);
    static GLenum to_gl(CullFace face);
    static GLenum to_gl(FrontFace face);
    static GLenum to_gl(BlendFactor factor);
    static GLenum to_gl(BlendEquation equation);

    Spec spec_;
};

} // namespace mini::gfx
