#pragma once
#include <cstdint>
#include <glad/glad.h>
#include <initializer_list>
#include <vector>

namespace mini::gl {

struct VertexAttribute {
    std::uint32_t location;
    std::int32_t  size;
    GLenum        type;
    bool          normalized;
    std::uint32_t offset;
};

class VertexLayout {
public:
    VertexLayout() = default;

    VertexLayout(std::initializer_list<VertexAttribute> attributes, std::uint32_t stride)
            : attributes_(attributes),
              stride_(stride) {}

    VertexLayout& add(std::uint32_t location, std::int32_t size, GLenum type, bool normalized,
            std::uint32_t offset) {
        attributes_.push_back({ location, size, type, normalized, offset });
        return *this;
    }

    std::uint32_t stride() const { return stride_; }
    VertexLayout& set_stride(std::uint32_t stride) {
        stride_ = stride;
        return *this;
    }

    const std::vector<VertexAttribute>& attributes() const { return attributes_; }

private:
    std::vector<VertexAttribute> attributes_;
    std::uint32_t stride_ = 0;
};

} 
