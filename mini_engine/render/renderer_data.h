#pragma once
#include "glm/ext/matrix_float4x4.hpp"
#include "render/camera.h"
#include "render/resources/resources_handle.h"

#include <glm/glm.hpp>
#include <optional>
#include <vector>

namespace mini::render {

struct DirectionalLight {
    glm::vec3 direction_{ 0.0f, -1.0f, 0.0f };
    glm::vec3 color_{ 1.0f };
    float intensity_ = 1.0f;
};

struct Environment {
    glm::vec3 ambient_color_{ 0.1f };
    glm::vec4 clear_color_{ 0.1f, 0.1f, 0.1f, 1.0f };
};

struct RenderItem {
    MeshHandle mesh_;
    MaterialHandle material_;
    glm::mat4 world_transform_{ 1.0f };
};

struct RenderData {
    CameraData camera_;
    Environment environment_;
    std::optional<DirectionalLight> directional_light_;
    std::vector<RenderItem> items_;
};
} // namespace mini::render