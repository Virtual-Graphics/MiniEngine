#include "asset/obj_loader.h"
#include "common/imgui_integration.h"
#include "engine/application.h"
#include "engine/layer.h"
#include "render/renderer.h"
#include "render/resources/mesh_data.h"
#include "scene/components.h"
#include "scene/scene.h"
#include "scene/scene_extractor.h"

#include <cmath>
#include <cstddef>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <imgui.h>
#include <memory>

class ModelViewerLayer : public engine::Layer {
public:
    ModelViewerLayer()
            : engine::Layer("ModelViewerLayer") {}

    void on_attach() override {
        const std::filesystem::path assets_dir = ASSETS_DIR;
        const mini::MeshData mesh_data = mini::asset::OBJImporter::load_obj(assets_dir / "StanfordBunny.obj");
        vertex_count_ = mesh_data.vertices_.size();
        triangle_count_ = mesh_data.indices_.size() / 3;

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

        environment_entity_ = scene_->create_entity("Environment");
        auto& environment = environment_entity_.add_component<mini::scene::EnvironmentComponent>();
        environment.ambient_color_ = glm::vec3{ 0.15f };
        environment.clear_color_ = glm::vec4{ 0.08f, 0.09f, 0.11f, 1.0f };

        light_entity_ = scene_->create_entity("Directional Light");
        auto& light = light_entity_.add_component<mini::scene::LightComponent>();
        light.direction_ = glm::vec3{ -1.0f, -1.0f, -1.0f };
        light.color_ = glm::vec3{ 1.0f };
        light.intensity_ = 1.0f;
        light.shadow_map_size_ = 2048;
        light.shadow_center_ = glm::vec3{ 0.0f, -0.5f, 0.0f };
        light.shadow_extent_ = 8.0f;
        light.shadow_near_ = 0.1f;
        light.shadow_far_ = 40.0f;

        camera_entity_ = scene_->create_entity("Main Camera");
        auto& camera_transform = camera_entity_.get_component<mini::scene::TransformComponent>();
        const glm::vec3 camera_position{ 0.0f, 4.0f, 8.0f };
        const glm::mat4 camera_world =
                glm::inverse(glm::lookAt(camera_position, glm::vec3{ 0.0f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }));
        camera_transform.translation_ = glm::vec3{ camera_world[3] };
        camera_transform.rotation_ = glm::eulerAngles(glm::quat_cast(camera_world));

        auto& camera = camera_entity_.add_component<mini::scene::CameraComponent>();
        camera.primary_ = true;
        camera.perspective_fov_y_ = glm::radians(45.0f);
        camera.perspective_near_ = 0.1f;
        camera.perspective_far_ = 100.0f;

        update_camera_aspect();
        imgui_ = std::make_unique<apps::ImGuiIntegration>(application().get_window());
    }

    void on_update(base::Timestep dt) override {
        imgui_->begin_frame();
        draw_ui();
        imgui_->end_frame();

        if (rotate_model_) {
            angle_ = std::fmod(angle_ + dt.get_seconds() * rotate_speed_, 360.0f);
        }

        auto& bunny_transform = bunny_entity_.get_component<mini::scene::TransformComponent>();
        bunny_transform.rotation_.y = glm::radians(angle_);

        update_camera_aspect();
    }

    void on_detach() override { imgui_.reset(); }

    void on_render() override {
        renderer_->render(extractor_.extract(*scene_));
        imgui_->render();
    }

private:
    void draw_ui() {
        ImGui::SetNextWindowPos(ImVec2{ 16.0f, 16.0f }, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2{ 340.0f, 0.0f }, ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Model Viewer", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            const float fps = ImGui::GetIO().Framerate;
            ImGui::Text("%.1f FPS | %.2f ms/frame", fps, fps > 0.0f ? 1000.0f / fps : 0.0f);
            ImGui::TextDisabled("Whole-frame interval, including VSync");
            ImGui::Text("Model: Stanford Bunny");
            ImGui::Text("Vertices: %zu | Triangles: %zu", vertex_count_, triangle_count_);

            ImGui::SeparatorText("Model");
            ImGui::Checkbox("Auto rotate", &rotate_model_);
            ImGui::SliderFloat("Speed (deg/s)", &rotate_speed_, 0.0f, 180.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

            auto& light = light_entity_.get_component<mini::scene::LightComponent>();
            if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
                glm::vec3 direction = light.direction_;
                if (ImGui::DragFloat3("Direction", &direction.x, 0.01f, -1.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp) &&
                        glm::dot(direction, direction) > 1e-6f) {
                    light.direction_ = direction;
                }
                ImGui::ColorEdit3("Light color", &light.color_.x);
                ImGui::SliderFloat("Intensity", &light.intensity_, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            }

            if (ImGui::CollapsingHeader("Environment", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& environment = environment_entity_.get_component<mini::scene::EnvironmentComponent>();
                ImGui::ColorEdit3("Ambient", &environment.ambient_color_.x);
                ImGui::ColorEdit3("Background", &environment.clear_color_.x);
            }

            if (ImGui::CollapsingHeader("Shadows", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Enabled", &light.shadow_enabled_);
                ImGui::BeginDisabled(!light.shadow_enabled_);

                constexpr std::uint32_t sizes[]{ 512, 1024, 2048, 4096 };
                constexpr const char* labels[]{ "512", "1024", "2048", "4096" };
                int selected = 0;
                for (int i = 0; i < 4; ++i) {
                    if (sizes[i] == light.shadow_map_size_) {
                        selected = i;
                    }
                }
                if (ImGui::Combo("Resolution", &selected, labels, 4)) {
                    light.shadow_map_size_ = sizes[selected];
                }

                ImGui::DragFloat("Depth bias", &light.shadow_depth_bias_, 0.00001f, 0.0f, 0.05f, "%.5f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::DragFloat("Normal bias", &light.shadow_normal_bias_, 0.001f, 0.0f, 0.2f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::EndDisabled();
            }
        }
        ImGui::End();
    }

    void update_camera_aspect() {
        auto& window = application().get_window();
        auto& camera = camera_entity_.get_component<mini::scene::CameraComponent>();
        camera.set_aspect(static_cast<float>(window.width()) / static_cast<float>(window.height()));
    }

    std::unique_ptr<mini::render::Renderer> renderer_;
    std::unique_ptr<mini::scene::Scene> scene_;
    mini::scene::Entity bunny_entity_;
    mini::scene::Entity camera_entity_;
    mini::scene::Entity light_entity_;
    mini::scene::Entity environment_entity_;
    mini::scene::SceneExtractor extractor_;
    std::unique_ptr<apps::ImGuiIntegration> imgui_;
    std::size_t vertex_count_ = 0;
    std::size_t triangle_count_ = 0;
    bool rotate_model_ = true;
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
