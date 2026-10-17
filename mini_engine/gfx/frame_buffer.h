#pragma once
#include "image.h"
#include "object.h"
#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace mini::gfx {

class FrameBuffer : public Object {
public:
    FrameBuffer()
            : Object([](GLuint id) { glDeleteFramebuffers(1, &id); }) {
        glCreateFramebuffers(1, &id_);
    }

    void attach_color(const Image& image, std::uint32_t index = 0) {
        glNamedFramebufferTexture(id_, GL_COLOR_ATTACHMENT0 + index, image.id(), 0);
    }

    void attach_depth(const Image& image) {
        glNamedFramebufferTexture(id_, GL_DEPTH_ATTACHMENT, image.id(), 0);
    }

    void attach_depth_stencil(const Image& image) {
        glNamedFramebufferTexture(id_, GL_DEPTH_STENCIL_ATTACHMENT, image.id(), 0);
    }

    void set_draw_buffers(const std::vector<std::uint32_t>& indices) {
        std::vector<GLenum> attachments;
        attachments.reserve(indices.size());
        for (std::uint32_t index: indices) {
            attachments.push_back(GL_COLOR_ATTACHMENT0 + index);
        }
        glNamedFramebufferDrawBuffers(id_, static_cast<GLsizei>(attachments.size()),
                attachments.data());
    }

    void set_draw_buffers_none() {
        glNamedFramebufferDrawBuffer(id_, GL_NONE);
        glNamedFramebufferReadBuffer(id_, GL_NONE);
    }

    GLenum status() const { return glCheckNamedFramebufferStatus(id_, GL_FRAMEBUFFER); }

    bool complete() const { return status() == GL_FRAMEBUFFER_COMPLETE; }

    static const char* status_name(GLenum status) {
        switch (status) {
            case GL_FRAMEBUFFER_COMPLETE:
                return "complete";
            case GL_FRAMEBUFFER_UNDEFINED:
                return "undefined";
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                return "incomplete attachment";
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                return "missing attachment";
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                return "incomplete draw buffer";
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                return "incomplete read buffer";
            case GL_FRAMEBUFFER_UNSUPPORTED:
                return "unsupported";
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                return "incomplete multisample";
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                return "incomplete layer targets";
            default:
                return "unknown";
        }
    }

    void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, id_); }
    static void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    void clear_color(const glm::vec4& color, std::uint32_t index = 0) const {
        glClearNamedFramebufferfv(id_, GL_COLOR, static_cast<GLint>(index), glm::value_ptr(color));
    }

    void clear_depth(float depth = 1.0f) const {
        glClearNamedFramebufferfv(id_, GL_DEPTH, 0, &depth);
    }

    void clear_stencil(std::int32_t stencil = 0) const {
        glClearNamedFramebufferiv(id_, GL_STENCIL, 0, &stencil);
    }
};

} // namespace mini::gfx
