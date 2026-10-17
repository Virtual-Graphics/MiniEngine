#include "scene/components.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace mini::scene {

glm::mat4 TransformComponent::matrix() const {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation_);
    transform *= glm::mat4_cast(glm::quat(rotation_));
    return glm::scale(transform, scale_);
}

void CameraComponent::set_aspect(float aspect) { perspective_spec_.aspect_ = aspect; }

render::CameraData CameraComponent::data(const glm::mat4& world_transform) const {
    if (projection_type_ == ProjectionType::Perspective) {
        return render::PerspectiveCamera{ perspective_spec_ }.data(world_transform);
    }
    return render::OrthographicCamera{ orthographic_spec_ }.data(world_transform);
}

} // namespace mini::scene
