#include "gfx/render_target.h"

#include "base/logger.h"
#include <stdexcept>

namespace mini::gfx {

namespace {

bool is_depth_format(Image::Format format) {
    return format == Image::Format::Depth24Stencil8 || format == Image::Format::Depth32F;
}

} // namespace

RenderTarget::RenderTarget(const RenderTargetSpec& spec)
        : spec_(spec) {
    if (spec_.width == 0 || spec_.height == 0) {
        throw std::invalid_argument("RenderTarget size must be non-zero");
    }
    if (spec_.with_depth && !is_depth_format(spec_.depth_format)) {
        throw std::invalid_argument("RenderTarget depth format is not a depth format");
    }
    create();
}

void RenderTarget::resize(std::uint32_t width, std::uint32_t height) {
    if (width == 0 || height == 0 || (width == spec_.width && height == spec_.height)) {
        return;
    }
    spec_.width = width;
    spec_.height = height;
    create();
}

void RenderTarget::bind() const {
    frame_buffer_.bind();
    glViewport(0, 0, static_cast<GLsizei>(spec_.width), static_cast<GLsizei>(spec_.height));
}

void RenderTarget::unbind() { FrameBuffer::unbind(); }

void RenderTarget::clear(const glm::vec4& color, float depth) const {
    clear_color(color);
    clear_depth(depth);
}

void RenderTarget::clear_color(const glm::vec4& color) const { frame_buffer_.clear_color(color); }

void RenderTarget::clear_depth(float depth) const {
    if (depth_) {
        frame_buffer_.clear_depth(depth);
    }
}

void RenderTarget::create() {
    color_ = std::make_unique<Texture>(Image(spec_.color_format, spec_.width, spec_.height), Sampler(spec_.sampler));
    frame_buffer_.attach_color(color_->image());
    frame_buffer_.set_draw_buffers({ 0 });

    if (spec_.with_depth) {
        depth_ = std::make_unique<Image>(spec_.depth_format, spec_.width, spec_.height);
        if (spec_.depth_format == Image::Format::Depth24Stencil8) {
            frame_buffer_.attach_depth_stencil(*depth_);
        } else {
            frame_buffer_.attach_depth(*depth_);
        }
    }

    if (!frame_buffer_.complete()) {
        const GLenum status = frame_buffer_.status();
        base::log::error("RenderTarget incomplete: {} ({}x{})", FrameBuffer::status_name(status), spec_.width, spec_.height);
        throw std::runtime_error("RenderTarget framebuffer incomplete");
    }

    base::log::info("RenderTarget created: {}x{} (depth: {})", spec_.width, spec_.height, spec_.with_depth ? "on" : "off");
}

} // namespace mini::gfx
