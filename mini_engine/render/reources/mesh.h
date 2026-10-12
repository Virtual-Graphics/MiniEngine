#pragma once
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

namespace mini{
struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
};

class Mesh {
public:
    std::vector<Vertex> vertices_;
    std::vector<std::uint32_t> indices_;
};
}
