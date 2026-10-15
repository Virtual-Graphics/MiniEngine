#pragma once
#include "buffer.h"
#include <cstdint>
#include <glad/glad.h>

namespace mini::gfx {

class UniformBuffer : public Buffer {
public:
    UniformBuffer() = default;

    template<typename T>
    explicit UniformBuffer(const T& data, std::uint32_t binding, Usage usage = Usage::Dynamic) {
        set_data(data, binding, usage);
    }

    template<typename T>
    void set_data(const T& data, std::uint32_t binding, Usage usage = Usage::Dynamic) {
        Buffer::set_data(static_cast<const void*>(&data), sizeof(T), usage);
        bind_base(binding);
    }

    void bind_base(std::uint32_t binding) {
        binding_ = binding;
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, id_);
    }

    std::uint32_t binding() const { return binding_; }

private:
    std::uint32_t binding_ = 0;
};

}
