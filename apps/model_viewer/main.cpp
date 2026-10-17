#include "asset/obj_loader.h"
#include "engine/application.h"
#include "engine/layer.h"
#include "render/renderer.h"
#include "render/resources/mesh_data.h"
#include "scene/components.h"
#include "scene/scene.h"
#include "scene/scene_extractor.h"

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class ModelViewerLayer : public engine::Layer {
public:
    ModelViewerLayer()
            : engine::Layer("ModelViewerLayer") {}

    void on_attach() override {
        const std::filesystem::path assets_dir = ASSETS_DIR;
        const mini::MeshData mesh_data = mini::asset::OBJImporter::load_obj(assets_dir / "StanfordBunny.obj");

        renderer_ = std::make_unique<mini::render::Renderer>();
        const auto mesh = renderer_->create_mesh(mesh_data);

        mini::render::Material material;
        material.diffuse_ = glm::vec3{ 0.9f };
        material.specular_ = glm::vec3{ 0.08f };
        material.shininess_ = 8.0f;
        const auto material_handle = renderer_->create_material(material);

        mini::MeshData ground_data;
        const float half = 20.0f;
        const float y = -1.05f;
        ground_data.vertices_ = {
            { { -half, y, -half }, { 0.0f, 1.0f, 0.0f } },
            { { half, y, -half }, { 0.0f, 1.0f, 0.0f } },
            { { half, y, half }, { 0.0f, 1.0f, 0.0f } },
            { { -half, y, half }, { 0.0f, 1.0f, 0.0f } },
        };
        ground_data.indices_ = { 0, 1, 2, 2, 3, 0 };
        const auto ground_mesh = renderer_->create_mesh(ground_data);

        mini::render::Material ground_material;
        ground_material.diffuse_ = glm::vec3{ 0.55f, 0.57f, 0.6f };
        ground_material.specular_ = glm::vec3{ 0.02f };
        ground_material.shininess_ = 4.0f;
        const auto ground_material_handle = renderer_->create_material(ground_material);

        scene_ = std::make_unique<mini::scene::Scene>();

        bunny_entity_ = scene_->create_entity("Bunny");
        auto& bunny_transform = bunny_entity_.get_component<mini::scene::TransformComponent>();
        bunny_transform.translation_ = glm::vec3{ 0.0f, -1.0f, 0.0f };
        bunny_transform.scale_ = glm::vec3{ 15.0f };
        auto& bunny_renderer = bunny_entity_.add_component<mini::scene::MeshRendererComponent>();
        bunny_renderer.mesh_ = mesh;
        bunny_renderer.material_ = material_handle;

        auto ground_entity = scene_->create_entity("Ground");
        auto& ground_renderer = ground_entity.add_component<mini::scene::MeshRendererComponent>();
        ground_renderer.mesh_ = ground_mesh;
        ground_renderer.material_ = ground_material_handle;

        auto environment_entity = scene_->create_entity("Environment");
        auto& environment = environment_entity.add_component<mini::scene::EnvironmentComponent>();
        environment.environment_.ambient_color_ = glm::vec3{ 0.15f };
        environment.environment_.clear_color_ = glm::vec4{ 0.08f, 0.09f, 0.11f, 1.0f };

        mini::render::DirectionalLight light;
        light.direction_ = glm::vec3{ -1.0f, -1.0f, -1.0f };
        light.color_ = glm::vec3{ 1.0f };
        light.intensity_ = 1.0f;
        light.shadow_.map_size_ = 2048;
        light.shadow_.center_ = glm::vec3{ 0.0f, -0.5f, 0.0f };
        light.shadow_.extent_ = 8.0f;
        light.shadow_.near_ = 0.1f;
        light.shadow_.far_ = 40.0f;
        environment.directional_light_ = light;

        camera_entity_ = scene_->create_entity("Main Camera");
        auto& camera_transform = camera_entity_.get_component<mini::scene::TransformComponent>();
        const glm::vec3 camera_position{ 0.0f, 4.0f, 8.0f };
        const glm::mat4 camera_world =
                glm::inverse(glm::lookAt(camera_position, glm::vec3{ 0.0f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }));
        camera_transform.translation_ = glm::vec3{ camera_world[3] };
        camera_transform.rotation_ = glm::eulerAngles(glm::quat_cast(camera_world));

        auto& camera = camera_entity_.add_component<mini::scene::CameraComponent>();
        camera.primary_ = true;
        camera.perspective_spec_.fov_y_ = glm::radians(45.0f);
        camera.perspective_spec_.near_ = 0.1f;
        camera.perspective_spec_.far_ = 100.0f;

        update_camera_aspect();
    }

    void on_update(base::Timestep dt) override {
        angle_ += dt * rotate_speed_;
        if (angle_ > 360.0f) {
            angle_ -= 360.0f;
        }

        auto& bunny_transform = bunny_entity_.get_component<mini::scene::TransformComponent>();
        bunny_transform.rotation_.y = glm::radians(angle_);

        update_camera_aspect();
    }

    void on_detach() override {}

    void on_render() override { renderer_->render(extractor_.extract(*scene_)); }

private:
    void update_camera_aspect() {
        auto& window = application().get_window();
        auto& camera = camera_entity_.get_component<mini::scene::CameraComponent>();
        camera.set_aspect(static_cast<float>(window.width()) / static_cast<float>(window.height()));
    }

    std::unique_ptr<mini::render::Renderer> renderer_;
    std::unique_ptr<mini::scene::Scene> scene_;
    mini::scene::Entity bunny_entity_;
    mini::scene::Entity camera_entity_;
    mini::scene::SceneExtractor extractor_;
    float angle_ = 0.0f;
    float rotate_speed_ = 60.0f;
};

int main() {
    engine::ApplicationCreateInfo info;
    info.width = 1280;
    info.height = 720;
    info.title = "MiniEngine - Model Viewer";

    engine::Application app(info);
    app.push_layer(std::make_unique<ModelViewerLayer>());
    app.run();
    return 0;
}
