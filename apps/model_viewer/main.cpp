#include "asset/obj_loader.h"
#include "engine/application.h"
#include "engine/layer.h"
#include "render/camera.h"
#include "render/renderer.h"
#include "render/render_data.h"
#include "render/resources/mesh_data.h"

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

class ModelViewerLayer : public engine::Layer {
public:
    ModelViewerLayer() : engine::Layer("ModelViewerLayer") {}

    void on_attach() override {
        const std::filesystem::path assets_dir = ASSETS_DIR;
        const mini::MeshData mesh_data = mini::asset::OBJImporter::load_obj(assets_dir / "StanfordBunny.obj");

        renderer_ = std::make_unique<mini::render::Renderer>();
        mesh_ = renderer_->create_mesh(mesh_data);

        mini::render::Material material;
        material.diffuse_ = glm::vec3{ 0.9f };
        material.specular_ = glm::vec3{ 0.08f };
        material.shininess_ = 8.0f;
        material_ = renderer_->create_material(material);

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
        ground_mesh_ = renderer_->create_mesh(ground_data);

        mini::render::Material ground_material;
        ground_material.diffuse_ = glm::vec3{ 0.55f, 0.57f, 0.6f };
        ground_material.specular_ = glm::vec3{ 0.02f };
        ground_material.shininess_ = 4.0f;
        ground_material_ = renderer_->create_material(ground_material);

        camera_ = std::make_unique<mini::render::PerspectiveCamera>();
        update_camera();
    }

    void on_update(base::Timestep dt) override {
        angle_ += dt * rotate_speed_;
        if (angle_ > 360.0f) {
            angle_ -= 360.0f;
        }

        model_ = glm::mat4(1.0f);
        model_ = glm::translate(model_, glm::vec3{ 0.0f, -1.0f, 0.0f });
        model_ = glm::rotate(model_, glm::radians(angle_), glm::vec3{ 0.0f, 1.0f, 0.0f });
        model_ = glm::scale(model_, glm::vec3(15.0f));

        update_camera();
    }

    void on_detach() override {}

    void on_render() override {
        mini::render::RenderData data;
        const glm::mat4 camera_world = glm::inverse(glm::lookAt(
                camera_position_, glm::vec3{ 0.0f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }));
        data.camera_ = camera_->data(camera_world);
        data.environment_.ambient_color_ = glm::vec3{ 0.15f };
        data.environment_.clear_color_ = clear_color_;

        mini::render::DirectionalLight light;
        light.direction_ = glm::vec3{ -1.0f, -1.0f, -1.0f };
        light.color_ = glm::vec3{ 1.0f };
        light.intensity_ = 1.0f;
        light.shadow_.map_size_ = 2048;
        light.shadow_.center_ = glm::vec3{ 0.0f, -0.5f, 0.0f };
        light.shadow_.extent_ = 8.0f;
        light.shadow_.near_ = 0.1f;
        light.shadow_.far_ = 40.0f;
        data.directional_light_ = light;

        data.items_.push_back(mini::render::RenderItem{ mesh_, material_, model_ });
        data.items_.push_back(mini::render::RenderItem{ ground_mesh_, ground_material_, glm::mat4(1.0f) });

        renderer_->render(data);
    }

private:
    void update_camera() {
        auto& window = application().get_window();

        mini::render::PerspectiveCamera::Spec spec;
        spec.fov_y_ = glm::radians(45.0f);
        spec.aspect_ = static_cast<float>(window.width()) / static_cast<float>(window.height());
        spec.near_ = 0.1f;
        spec.far_ = 100.0f;
        camera_->set_spec(spec);
    }

    std::unique_ptr<mini::render::Renderer> renderer_;
    std::unique_ptr<mini::render::PerspectiveCamera> camera_;
    mini::render::MeshHandle mesh_;
    mini::render::MaterialHandle material_;
    mini::render::MeshHandle ground_mesh_;
    mini::render::MaterialHandle ground_material_;
    glm::vec3 camera_position_{ 0.0f, 4.0f, 8.0f };
    glm::mat4 model_{ 1.0f };
    float angle_ = 0.0f;
    float rotate_speed_ = 60.0f;
    glm::vec4 clear_color_{ 0.08f, 0.09f, 0.11f, 1.0f };
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
