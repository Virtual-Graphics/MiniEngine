#include "gfx/image.h"

namespace mini::gfx {

namespace {

struct FormatInfo {
    GLenum internal_format;
    GLenum pixel_format;
    GLenum pixel_type;
};

FormatInfo format_info(Image::Format format) {
    switch (format) {
    case Image::Format::R8:               return { GL_R8,               GL_RED,  GL_UNSIGNED_BYTE };
    case Image::Format::RGB8:             return { GL_RGB8,             GL_RGB,  GL_UNSIGNED_BYTE };
    case Image::Format::RGBA8:            return { GL_RGBA8,            GL_RGBA, GL_UNSIGNED_BYTE };
    case Image::Format::SRGB8Alpha8:      return { GL_SRGB8_ALPHA8,      GL_RGBA, GL_UNSIGNED_BYTE };
    case Image::Format::R16F:             return { GL_R16F,             GL_RED,  GL_HALF_FLOAT };
    case Image::Format::RGBA16F:          return { GL_RGBA16F,          GL_RGBA, GL_HALF_FLOAT };
    case Image::Format::R32F:             return { GL_R32F,             GL_RED,  GL_FLOAT };
    case Image::Format::RGBA32F:          return { GL_RGBA32F,          GL_RGBA, GL_FLOAT };
    case Image::Format::Depth24Stencil8:  return { GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 };
    case Image::Format::Depth32F:         return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT };
    }
    return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
}

}

Image::Image(Format format, std::uint32_t width, std::uint32_t height)
        : Object([](GLuint id) { glDeleteTextures(1, &id); }),
          width_(width),
          height_(height),
          format_(format) {
    glCreateTextures(GL_TEXTURE_2D, 1, &id_);

    const FormatInfo info = format_info(format);
    glTextureStorage2D(id_, 1, info.internal_format, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

    glTextureParameteri(id_, GL_TEXTURE_BASE_LEVEL, 0);
    glTextureParameteri(id_, GL_TEXTURE_MAX_LEVEL, 0);
}

void Image::set_data(const void* pixels, std::uint32_t mip_level) {
    const FormatInfo info = format_info(format_);
    glTextureSubImage2D(id_, static_cast<GLint>(mip_level), 0, 0,
                        static_cast<GLsizei>(width_), static_cast<GLsizei>(height_),
                        info.pixel_format, info.pixel_type, pixels);
}

}
