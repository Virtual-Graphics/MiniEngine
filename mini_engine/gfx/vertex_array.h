#pragma once
#include "index_buffer.h"
#include "object.h"
#include "vertex_buffer.h"
#include "vertex_layout.h"
#include <cstddef>
#include <glad/glad.h>

namespace mini::gfx {

class VertexArray : public Object {
public:
    VertexArray() : Object([](GLuint id) { glDeleteVertexArrays(1, &id); }) {
        glCreateVertexArrays(1, &id_);
    }
    ~VertexArray() = default;

    void set_vertex_buffer(const VertexBuffer& vbo, const VertexLayout& layout) {
        glVertexArrayVertexBuffer(id_, 0, vbo.id(), 0, layout.stride());
        for (const auto& a: layout.attributes()) {
            glEnableVertexArrayAttrib(id_, a.location);
            glVertexArrayAttribFormat(id_, a.location, a.size, a.type, a.normalized, a.offset);
            glVertexArrayAttribBinding(id_, a.location, 0);
        }
    }

    void set_index_buffer(const IndexBuffer& ibo) {
        index_count_ = ibo.count();
        index_type_ = ibo.type();
        glVertexArrayElementBuffer(id_, ibo.id());
    }

    void bind() const { glBindVertexArray(id_); }
    static void unbind() { glBindVertexArray(0); }

    std::size_t index_count() const { return index_count_; }
    IndexBuffer::Type index_type() const { return index_type_; }

private:
    std::size_t index_count_ = 0;
    IndexBuffer::Type index_type_ = IndexBuffer::Type::Uint32;
};

} 
