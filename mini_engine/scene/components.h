#pragma once

#include "base/uuid.h"
#include "render/camera.h"
#include "render/render_data.h"
#include "render/resources/resources_handle.h"

#include <glm/glm.hpp>
#include <optional>
#include <string>

namespace mini::scene {

struct IDComponent {
    base::UUID id_ = base::UUID::generate();
};

struct TagComponent {
    std::string name_ = "Entity";
};

struct TransformComponent {
    glm::vec3 translation_{ 0.0f };
    glm::vec3 rotation_{ 0.0f };
    glm::vec3 scale_{ 1.0f };

    glm::mat4 matrix() const;
};

struct MeshRendererComponent {
    render::MeshHandle mesh_;
    render::MaterialHandle material_;
    bool visible_ = true;
};

struct CameraComponent {
    enum class ProjectionType { Perspective, Orthographic };

    ProjectionType projection_type_ = ProjectionType::Perspective;
    bool primary_ = true;
    render::PerspectiveCamera::Spec perspective_spec_;
    render::OrthographicCamera::Spec orthographic_spec_;

    void set_aspect(float aspect);
    render::CameraData data(const glm::mat4& world_transform) const;
};

struct EnvironmentComponent {
    render::Environment environment_;
    std::optional<render::DirectionalLight> directional_light_;
};

} // namespace mini::scene
