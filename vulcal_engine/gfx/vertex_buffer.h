#pragma once
#include "buffer.h"
#include <vector>
#include <glad/glad.h>

namespace vulcal::gfx{
class VertexBuffer : public Buffer {
public:

    VertexBuffer() = default;
    
    template<typename T>
    explicit VertexBuffer(const std::vector<T>& data, Usage usage = Usage::Static){
        this->set_data(data,usage);
    }

    explicit VertexBuffer(const void* data, std::size_t size, Usage usage = Usage::Static) {
        this->set_data(data, size, usage);
    }
    
    template<typename T>
    void set_data(const std::vector<T>& data, Usage usage = Usage::Static){
        Buffer::set_data(data,usage);
    }

    void set_data(const void* data, std::size_t size, Usage usage = Usage::Static) {
        Buffer::set_data(data, size, usage);
    }
    virtual ~VertexBuffer() = default;

};
}
