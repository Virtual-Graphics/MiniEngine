#include "gfx/sampler.h"

namespace mini::gfx {

GLenum Sampler::to_gl(Filter filter) {
    switch (filter) {
    case Filter::Nearest:              return GL_NEAREST;
    case Filter::Linear:               return GL_LINEAR;
    case Filter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
    case Filter::LinearMipmapNearest:  return GL_LINEAR_MIPMAP_NEAREST;
    case Filter::NearestMipmapLinear:  return GL_NEAREST_MIPMAP_LINEAR;
    case Filter::LinearMipmapLinear:   return GL_LINEAR_MIPMAP_LINEAR;
    }
    return GL_LINEAR;
}

GLenum Sampler::to_gl(Wrap wrap) {
    switch (wrap) {
    case Wrap::Repeat:         return GL_REPEAT;
    case Wrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
    case Wrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
    case Wrap::ClampToBorder:  return GL_CLAMP_TO_BORDER;
    }
    return GL_REPEAT;
}

Sampler::Sampler(const Spec& spec)
        : Object([](GLuint id) { glDeleteSamplers(1, &id); }) {
    glCreateSamplers(1, &id_);

    glSamplerParameteri(id_, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(to_gl(spec.min_filter)));
    glSamplerParameteri(id_, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(to_gl(spec.mag_filter)));
    glSamplerParameteri(id_, GL_TEXTURE_WRAP_S, static_cast<GLint>(to_gl(spec.wrap_s)));
    glSamplerParameteri(id_, GL_TEXTURE_WRAP_T, static_cast<GLint>(to_gl(spec.wrap_t)));
    glSamplerParameteri(id_, GL_TEXTURE_WRAP_R, static_cast<GLint>(to_gl(spec.wrap_r)));
}

}
