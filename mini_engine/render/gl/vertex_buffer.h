#pragma once
#include "buffer.h"
#include <vector>
#include <glad/glad.h>

namespace mini::gl{
class VertexBuffer : public Buffer {
public:

    VertexBuffer() = default;
    
    template<typename T>
    explicit VertexBuffer(const std::vector<T>& data, Usage usage = Usage::Static){
        set_data(data,usage);
    }
    explicit VertexBuffer(const void* data, std::size_t size, Usage usage = Usage::Static) {
        set_data(data, size, usage);
    }

    
    virtual ~VertexBuffer() = default;

};
}
