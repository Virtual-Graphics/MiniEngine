#pragma once
#include "frame_buffer.h"
#include "image.h"
#include "sampler.h"
#include "texture.h"
#include <cstdint>
#include <memory>
#include <optional>

namespace mini::gfx {

class RenderTarget {
public:
    struct Spec {
        std::uint32_t width = 1;
        std::uint32_t height = 1;
        // A depth-only target leaves color_format empty (shadow maps, etc.).
        std::optional<Image::Format> color_format = Image::Format::RGBA8;
        std::optional<Image::Format> depth_format = Image::Format::Depth24Stencil8;
        Sampler::Spec sampler = Sampler::Spec::clamp_to_edge();
        std::optional<Sampler::Spec> depth_sampler;
    };

    explicit RenderTarget(const Spec& spec);

    void resize(std::uint32_t width, std::uint32_t height);

    void bind() const;
    static void unbind();

    void clear(const glm::vec4& color, float depth = 1.0f) const;
    void clear_color(const glm::vec4& color) const;
    void clear_depth(float depth = 1.0f) const;

    const Texture* color() const { return color_.get(); }
    const Texture* depth() const { return depth_.get(); }
    const FrameBuffer& frame_buffer() const { return frame_buffer_; }

    std::uint32_t width() const { return spec_.width; }
    std::uint32_t height() const { return spec_.height; }

private:
    void create();

    Spec spec_;
    std::unique_ptr<Texture> color_;
    std::unique_ptr<Texture> depth_;
    FrameBuffer frame_buffer_;
    bool created_before_ = false;
};

} // namespace mini::gfx
