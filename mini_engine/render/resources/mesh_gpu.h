#pragma once
#include "gfx/index_buffer.h"
#include "gfx/vertex_buffer.h"
#include "gfx/vertex_array.h"
#include "render/resources/mesh_data.h"

namespace mini::render{
class MeshGPU {
public:
    explicit MeshGPU(const MeshData& data){
        vertex_buffer_.set_data(data.vertices_);
        index_buffer_.set_data(data.indices_, gfx::IndexBuffer::Type::Uint32);

        const gfx::VertexLayout layout{
            { 
              { 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position) },
              { 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal)   }
            },
            sizeof(Vertex)
        };

        vertex_array_.set_vertex_buffer(vertex_buffer_, layout);
        vertex_array_.set_index_buffer(index_buffer_);
    }

    const gfx::VertexArray& vertex_array() const { return vertex_array_; }
private:
    gfx::VertexBuffer vertex_buffer_;
    gfx::IndexBuffer index_buffer_;
    gfx::VertexArray vertex_array_;
};
}