#include "editor/default_scene.h"

#include "asset/obj_loader.h"
#include "render/renderer.h"
#include "scene/scene.h"

#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace editor {

std::unique_ptr<mini::scene::Scene> create_default_scene(mini::render::Renderer& renderer) {
    auto scene = std::make_unique<mini::scene::Scene>();
    const auto mesh_data = mini::asset::OBJImporter::load_obj(std::filesystem::path{ ASSETS_DIR } / "StanfordBunny.obj");
    const auto mesh = renderer.create_mesh(mesh_data);
    mini::render::Material material;
    material.diffuse_ = glm::vec3{ 0.9f };
    material.specular_ = glm::vec3{ 0.08f };
    material.shininess_ = 8.0f;

    auto bunny = scene->create_entity("Bunny");
    auto& transform = bunny.get_component<mini::scene::TransformComponent>();
    transform.translation_ = { 0.0f, -1.0f, 0.0f };
    transform.scale_ = glm::vec3{ 15.0f };
    auto& bunny_mesh = bunny.add_component<mini::scene::MeshRendererComponent>();
    bunny_mesh.mesh_ = mesh;
    bunny_mesh.material_ = renderer.create_material(material);

    mini::MeshData ground_data;
    ground_data.vertices_ = {
        { { -20.0f, -1.05f, -20.0f }, { 0.0f, 1.0f, 0.0f } },
        { { 20.0f, -1.05f, -20.0f }, { 0.0f, 1.0f, 0.0f } },
        { { 20.0f, -1.05f, 20.0f }, { 0.0f, 1.0f, 0.0f } },
        { { -20.0f, -1.05f, 20.0f }, { 0.0f, 1.0f, 0.0f } },
    };
    ground_data.indices_ = { 0, 1, 2, 2, 3, 0 };
    mini::render::Material ground_material;
    ground_material.diffuse_ = { 0.55f, 0.57f, 0.6f };
    ground_material.specular_ = glm::vec3{ 0.02f };
    ground_material.shininess_ = 4.0f;
    auto ground = scene->create_entity("Ground");
    auto& ground_mesh = ground.add_component<mini::scene::MeshRendererComponent>();
    ground_mesh.mesh_ = renderer.create_mesh(ground_data);
    ground_mesh.material_ = renderer.create_material(ground_material);

    auto environment_entity = scene->create_entity("Environment");
    auto& environment = environment_entity.add_component<mini::scene::EnvironmentComponent>();
    environment.ambient_color_ = glm::vec3{ 0.15f };
    environment.clear_color_ = { 0.08f, 0.09f, 0.11f, 1.0f };

    auto light_entity = scene->create_entity("Directional Light");
    auto& light = light_entity.add_component<mini::scene::LightComponent>();
    light_entity.get_component<mini::scene::TransformComponent>().rotation_ =
            glm::eulerAngles(glm::quatLookAt(glm::normalize(glm::vec3{ -1.0f }), glm::vec3{ 0.0f, 1.0f, 0.0f }));
    light.shadow_center_ = { 0.0f, -0.5f, 0.0f };
    light.shadow_extent_ = 8.0f;
    light.shadow_far_ = 40.0f;

    auto camera_entity = scene->create_entity("Main Camera");
    auto& camera_transform = camera_entity.get_component<mini::scene::TransformComponent>();
    const auto world =
            glm::inverse(glm::lookAt(glm::vec3{ 0.0f, 4.0f, 8.0f }, glm::vec3{ 0.0f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }));
    camera_transform.translation_ = glm::vec3{ world[3] };
    camera_transform.rotation_ = glm::eulerAngles(glm::quat_cast(world));
    camera_entity.add_component<mini::scene::CameraComponent>();
    return scene;
}

} // namespace editor
