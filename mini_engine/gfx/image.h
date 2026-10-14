#pragma once
#include "object.h"
#include <cstdint>
#include <glad/glad.h>

namespace mini::gfx {

class Image : public Object {
public:
    enum class Format {
        R8,
        RGB8,
        RGBA8,
        SRGB8Alpha8,
        R16F,
        RGBA16F,
        R32F,
        RGBA32F,
        Depth24Stencil8,
        Depth32F,
    };

    Image(Format format, std::uint32_t width, std::uint32_t height);

    void set_data(const void* pixels, std::uint32_t mip_level = 0);
    void bind(std::uint32_t unit) const { glBindTextureUnit(unit, id_); }

    std::uint32_t width() const { return width_; }
    std::uint32_t height() const { return height_; }
    Format format() const { return format_; }

private:
    std::uint32_t width_ = 0;
    std::uint32_t height_ = 0;
    Format format_ = Format::RGBA8;
};

}
