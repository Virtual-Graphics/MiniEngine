#pragma once
#include "frame_buffer.h"
#include "image.h"
#include "sampler.h"
#include "texture.h"
#include <cstdint>
#include <memory>

namespace mini::gfx {

struct RenderTargetSpec {
    std::uint32_t width = 1;
    std::uint32_t height = 1;
    Image::Format color_format = Image::Format::RGBA8;
    bool with_depth = true;
    Image::Format depth_format = Image::Format::Depth24Stencil8;
    Sampler::Spec sampler = {
        .min_filter = Sampler::Filter::Linear,
        .mag_filter = Sampler::Filter::Linear,
        .wrap_s = Sampler::Wrap::ClampToEdge,
        .wrap_t = Sampler::Wrap::ClampToEdge,
        .wrap_r = Sampler::Wrap::ClampToEdge,
    };
};

class RenderTarget {
public:
    explicit RenderTarget(const RenderTargetSpec& spec);

    void resize(std::uint32_t width, std::uint32_t height);

    void bind() const;
    static void unbind();

    void clear(const glm::vec4& color, float depth = 1.0f) const;
    void clear_color(const glm::vec4& color) const;
    void clear_depth(float depth = 1.0f) const;

    const Texture& color() const { return *color_; }
    const Image* depth() const { return depth_.get(); }
    const FrameBuffer& frame_buffer() const { return frame_buffer_; }

    std::uint32_t width() const { return spec_.width; }
    std::uint32_t height() const { return spec_.height; }

private:
    void create();

    RenderTargetSpec spec_;
    std::unique_ptr<Texture> color_;
    std::unique_ptr<Image> depth_;
    FrameBuffer frame_buffer_;
};

} // namespace mini::gfx
