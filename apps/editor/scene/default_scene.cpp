#include "editor/scene/default_scene.h"
#include "scene/scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace editor {
std::unique_ptr<vulcal::scene::Scene> create_default_scene() {
    using namespace vulcal::scene;
    auto scene = std::make_unique<Scene>();
    auto sun = scene->create_entity("Sun");
    sun.add_component<LightComponent>();
    sun.get_component<TransformComponent>().rotation_ =
        glm::eulerAngles(glm::quatLookAt(glm::normalize(glm::vec3{ -1.0f }), glm::vec3{ 0, 1, 0 }));
    auto camera = scene->create_entity("Camera");
    camera.add_component<CameraComponent>();
    const auto world = glm::inverse(glm::lookAt(glm::vec3{ 0, 4, 8 }, glm::vec3{ 0 }, glm::vec3{ 0, 1, 0 }));
    camera.get_component<TransformComponent>().translation_ = glm::vec3{ world[3] };
    camera.get_component<TransformComponent>().rotation_ = glm::eulerAngles(glm::quat_cast(world));
    return scene;
}
} // namespace editor
