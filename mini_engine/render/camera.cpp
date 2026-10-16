#include "render/camera.h"
#include <glm/gtc/matrix_transform.hpp>


namespace mini::render {

Camera::Camera() = default;

Camera::~Camera() = default;

CameraData Camera::data(const glm::mat4& world_transform) const {
    CameraData result;
    result.view_ = glm::inverse(world_transform);
    result.projection_ = projection();
    result.position_ = glm::vec3(world_transform[3]);
    return result;
}

PerspectiveCamera::PerspectiveCamera() : PerspectiveCamera(Spec{}) {}

PerspectiveCamera::PerspectiveCamera(const Spec& spec) { set_spec(spec); }

void PerspectiveCamera::set_spec(const Spec& spec) {
    spec_ = spec;
}

glm::mat4 PerspectiveCamera::projection() const {
    return glm::perspective(spec_.fov_y_, spec_.aspect_, spec_.near_, spec_.far_);
}

OrthographicCamera::OrthographicCamera() : OrthographicCamera(Spec{}) {}

OrthographicCamera::OrthographicCamera(const Spec& spec) { set_spec(spec); }

void OrthographicCamera::set_spec(const Spec& spec) {
    spec_ = spec;
}

glm::mat4 OrthographicCamera::projection() const {
    return glm::ortho(spec_.left_, spec_.right_, spec_.bottom_, spec_.top_, spec_.near_, spec_.far_);
}

} // namespace mini::render
