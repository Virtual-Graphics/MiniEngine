#pragma once
#include <glm/glm.hpp>
namespace vulcal::render {

struct Material {
    glm::vec3 diffuse_{ 0.9f };
    glm::vec3 specular_{ 0.08f };
    float shininess_ = 8.0f;
};

}