#include "base/uuid.h"
#include "scene/components.h"
#include "scene/scene.h"
#include "scene/scene_extractor.h"

#include <cmath>
#include <cstddef>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace {

int failures = 0;

void check(bool condition, const char* message) {
    if (condition) {
        return;
    }

    std::cerr << "[FAIL] " << message << '\n';
    ++failures;
}

template<typename T>
void check_equal(const T& actual, const T& expected, const char* message) {
    if (actual == expected) {
        return;
    }

    std::cerr << "[FAIL] " << message << '\n';
    ++failures;
}

void check_near(float actual, float expected, const char* message, float epsilon = 1e-4f) {
    if (std::abs(actual - expected) <= epsilon) {
        return;
    }

    std::cerr << "[FAIL] " << message << '\n';
    ++failures;
}

void check_near(const glm::vec3& actual, const glm::vec3& expected, const char* message, float epsilon = 1e-4f) {
    if (std::abs(actual.x - expected.x) <= epsilon && std::abs(actual.y - expected.y) <= epsilon &&
            std::abs(actual.z - expected.z) <= epsilon) {
        return;
    }

    std::cerr << "[FAIL] " << message << '\n';
    ++failures;
}

mini::scene::Entity find_entity_by_id(mini::scene::Scene& scene, const base::UUID& id) {
    for (const auto entity: scene.registry().view<mini::scene::IDComponent>()) {
        if (scene.registry().get<mini::scene::IDComponent>(entity).id_ == id) {
            return mini::scene::Entity{ entity, scene };
        }
    }

    return {};
}

void test_uuid() {
    const base::UUID nil;
    check(nil.is_nil(), "default UUID should be nil");
    check_equal(nil.value(), 0ull, "nil UUID value should be zero");

    const auto first = base::UUID::generate();
    const auto second = base::UUID::generate();
    check(!first.is_nil(), "generated UUID should not be nil");
    check(!(first == second), "generated UUIDs should be different");
    check_equal(first.to_string().size(), static_cast<std::size_t>(16), "UUID string should contain 16 hex digits");
    check_equal(base::UUID{ 0x1234ull }.to_string(), std::string{ "0000000000001234" }, "UUID string should be zero padded");

    std::unordered_map<base::UUID, int> values;
    values.emplace(first, 1);
    values.emplace(second, 2);
    check_equal(values.size(), static_cast<std::size_t>(2), "UUID should be usable as an unordered_map key");
}

void test_scene_crud() {
    mini::scene::Scene scene;
    auto entity = scene.create_entity("Player");

    check(static_cast<bool>(entity), "created entity should be valid");
    check(entity.has_component<mini::scene::IDComponent>(), "entity should have IDComponent");
    check(entity.has_component<mini::scene::TagComponent>(), "entity should have TagComponent");
    check(entity.has_component<mini::scene::TransformComponent>(), "entity should have TransformComponent");
    check_equal(entity.get_component<mini::scene::TagComponent>().name_, std::string{ "Player" }, "tag name should match");

    const auto id = entity.get_component<mini::scene::IDComponent>().id_;
    auto& transform = entity.get_component<mini::scene::TransformComponent>();
    transform.translation_ = glm::vec3{ 1.0f, 2.0f, 3.0f };

    auto& mesh_renderer = entity.add_component<mini::scene::MeshRendererComponent>();
    mesh_renderer.mesh_ = mini::render::MeshHandle{ 7 };
    mesh_renderer.material_ = mini::render::MaterialHandle{ 9 };
    check(entity.has_component<mini::scene::MeshRendererComponent>(), "entity should have MeshRendererComponent");
    check_equal(entity.get_component<mini::scene::MeshRendererComponent>().mesh_.value_, 7ull, "mesh handle should match");

    entity.remove_component<mini::scene::MeshRendererComponent>();
    check(!entity.has_component<mini::scene::MeshRendererComponent>(), "MeshRendererComponent should be removed");

    auto second = scene.create_entity("Enemy");
    check(scene.has_component<mini::scene::IDComponent>(second), "scene should expose has_component");
    check(!(second.get_component<mini::scene::IDComponent>().id_ == id), "entity IDs should be unique");

    scene.remove_component<mini::scene::TagComponent>(second);
    check(!scene.has_component<mini::scene::TagComponent>(second), "scene remove_component should work");
    scene.add_component<mini::scene::TagComponent>(second, mini::scene::TagComponent{ "Boss" });
    check_equal(scene.get_component<mini::scene::TagComponent>(second).name_, std::string{ "Boss" }, "scene add/get_component should work");

    scene.destroy_entity(second);
    check(!second.valid(), "destroyed entity should be invalid");
    check(!static_cast<bool>(second), "destroyed entity should be false");
    check(entity.valid(), "destroying one entity should not affect another");
}

void test_transform_and_camera() {
    mini::scene::TransformComponent transform;
    transform.translation_ = glm::vec3{ 1.0f, 2.0f, 3.0f };
    transform.scale_ = glm::vec3{ 2.0f };

    const auto matrix = transform.matrix();
    check_near(glm::vec3{ matrix[3] }, glm::vec3{ 1.0f, 2.0f, 3.0f }, "transform matrix should contain translation");
    check_near(glm::vec3{ matrix * glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f } }, glm::vec3{ 3.0f, 2.0f, 3.0f },
            "transform matrix should apply scale");

    mini::scene::CameraComponent camera;
    camera.perspective_fov_y_ = glm::radians(60.0f);
    camera.perspective_aspect_ = 1.5f;
    camera.perspective_near_ = 0.1f;
    camera.perspective_far_ = 100.0f;

    camera.set_aspect(2.0f);
    check_near(camera.perspective_aspect_, 2.0f, "set_aspect should update the perspective aspect");

    const glm::mat4 camera_world =
            glm::inverse(glm::lookAt(glm::vec3{ 0.0f, 4.0f, 8.0f }, glm::vec3{ 0.0f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }));
    mini::scene::TransformComponent camera_transform;
    camera_transform.translation_ = glm::vec3{ camera_world[3] };
    camera_transform.rotation_ = glm::eulerAngles(glm::quat_cast(camera_world));

    const glm::mat3 expected_basis = glm::mat3{ camera_world };
    const glm::mat3 actual_basis = glm::mat3{ camera_transform.matrix() };
    check_near(glm::vec3{ actual_basis[0] }, glm::vec3{ expected_basis[0] }, "transform should round-trip camera orientation");
    check_near(glm::vec3{ actual_basis[1] }, glm::vec3{ expected_basis[1] }, "transform should round-trip camera orientation");
    check_near(glm::vec3{ actual_basis[2] }, glm::vec3{ expected_basis[2] }, "transform should round-trip camera orientation");
}

void test_scene_copy() {
    mini::scene::Scene scene;
    auto source = scene.create_entity("Source");
    source.get_component<mini::scene::TransformComponent>().translation_ = glm::vec3{ 4.0f, 5.0f, 6.0f };

    auto& source_renderer = source.add_component<mini::scene::MeshRendererComponent>();
    source_renderer.mesh_ = mini::render::MeshHandle{ 21 };
    source_renderer.material_ = mini::render::MaterialHandle{ 22 };

    auto& camera = source.add_component<mini::scene::CameraComponent>();
    camera.primary_ = true;
    camera.perspective_fov_y_ = glm::radians(50.0f);

    auto environment_entity = scene.create_entity("Environment");
    auto& environment = environment_entity.add_component<mini::scene::EnvironmentComponent>();
    environment.environment_.ambient_color_ = glm::vec3{ 0.2f, 0.3f, 0.4f };
    environment.directional_light_ = mini::render::DirectionalLight{};
    environment.directional_light_->intensity_ = 2.0f;

    const auto source_id = source.get_component<mini::scene::IDComponent>().id_;
    const auto source_translation = source.get_component<mini::scene::TransformComponent>().translation_;

    mini::scene::Scene copy = scene;
    auto copied_source = find_entity_by_id(copy, source_id);
    check(copied_source.valid(), "copied scene should contain the same entity ID");
    check(copied_source.get_component<mini::scene::IDComponent>().id_ == source_id, "scene copy should preserve entity UUID");
    check_equal(copied_source.get_component<mini::scene::TagComponent>().name_, std::string{ "Source" }, "scene copy should preserve tags");
    check_near(copied_source.get_component<mini::scene::TransformComponent>().translation_, source_translation,
            "scene copy should preserve transforms");
    check(copied_source.has_component<mini::scene::CameraComponent>(), "scene copy should preserve cameras");
    check(copied_source.get_component<mini::scene::CameraComponent>().primary_, "scene copy should preserve camera flags");
    check_equal(copied_source.get_component<mini::scene::MeshRendererComponent>().mesh_.value_, 21ull,
            "scene copy should preserve mesh handles");

    const auto environment_view = copy.registry().view<const mini::scene::EnvironmentComponent>();
    check(!environment_view.empty(), "scene copy should preserve environment components");
    if (!environment_view.empty()) {
        const auto& copied_environment = environment_view.get<const mini::scene::EnvironmentComponent>(environment_view.front());
        check_near(copied_environment.environment_.ambient_color_, glm::vec3{ 0.2f, 0.3f, 0.4f },
                "scene copy should preserve environment data");
        check(copied_environment.directional_light_.has_value(), "scene copy should preserve directional light");
    }

    copied_source.get_component<mini::scene::TransformComponent>().translation_.x = 100.0f;
    check_near(source.get_component<mini::scene::TransformComponent>().translation_.x, 4.0f, "copies should be independent");

    copy.destroy_entity(copied_source);
    check(!copied_source.valid(), "destroying a copied entity should invalidate the copy");
    check(source.valid(), "destroying a copied entity should not affect the source scene");

    mini::scene::Scene assigned;
    assigned = scene;
    auto assigned_source = find_entity_by_id(assigned, source_id);
    check(assigned_source.valid(), "copy assignment should preserve entities");
    check_near(assigned_source.get_component<mini::scene::TransformComponent>().translation_, source_translation,
            "copy assignment should preserve transforms");
}

void test_scene_extractor() {
    mini::scene::Scene scene;

    auto camera_entity = scene.create_entity("Camera");
    camera_entity.get_component<mini::scene::TransformComponent>().translation_ = glm::vec3{ 0.0f, 0.0f, 5.0f };
    auto& camera = camera_entity.add_component<mini::scene::CameraComponent>();
    camera.primary_ = true;
    camera.perspective_fov_y_ = glm::radians(45.0f);
    camera.perspective_aspect_ = 2.0f;
    camera.perspective_near_ = 0.1f;
    camera.perspective_far_ = 100.0f;

    auto environment_entity = scene.create_entity("Environment");
    auto& environment = environment_entity.add_component<mini::scene::EnvironmentComponent>();
    environment.environment_.ambient_color_ = glm::vec3{ 0.1f, 0.2f, 0.3f };
    environment.environment_.clear_color_ = glm::vec4{ 0.4f, 0.5f, 0.6f, 1.0f };
    environment.directional_light_ = mini::render::DirectionalLight{};
    environment.directional_light_->direction_ = glm::vec3{ -1.0f };
    environment.directional_light_->intensity_ = 3.0f;

    auto mesh_entity = scene.create_entity("Mesh");
    mesh_entity.get_component<mini::scene::TransformComponent>().translation_ = glm::vec3{ 1.0f, 2.0f, 3.0f };
    auto& mesh_renderer = mesh_entity.add_component<mini::scene::MeshRendererComponent>();
    mesh_renderer.mesh_ = mini::render::MeshHandle{ 11 };
    mesh_renderer.material_ = mini::render::MaterialHandle{ 22 };

    auto hidden_entity = scene.create_entity("Hidden");
    auto& hidden_renderer = hidden_entity.add_component<mini::scene::MeshRendererComponent>();
    hidden_renderer.mesh_ = mini::render::MeshHandle{ 33 };
    hidden_renderer.material_ = mini::render::MaterialHandle{ 44 };
    hidden_renderer.visible_ = false;

    mini::scene::SceneExtractor extractor;
    const auto data = extractor.extract(scene);

    check_near(data.camera_.position_, glm::vec3{ 0.0f, 0.0f, 5.0f }, "extractor should use the primary camera");
    check_near(glm::vec3{ data.camera_.view_[3] }, glm::vec3{ 0.0f, 0.0f, -5.0f }, "extracted camera view should match");
    const float expected_projection_00 = 1.0f / (camera.perspective_aspect_ * std::tan(camera.perspective_fov_y_ * 0.5f));
    check_near(data.camera_.projection_[0][0], expected_projection_00, "extracted perspective projection should match");
    check_equal(data.items_.size(), static_cast<std::size_t>(1), "extractor should skip invisible renderers");
    if (data.items_.size() == 1) {
        check_equal(data.items_[0].mesh_.value_, 11ull, "extracted mesh handle should match");
        check_equal(data.items_[0].material_.value_, 22ull, "extracted material handle should match");
        check_near(glm::vec3{ data.items_[0].world_transform_[3] }, glm::vec3{ 1.0f, 2.0f, 3.0f },
                "extracted world transform should match");
    }
    check_near(data.environment_.ambient_color_, glm::vec3{ 0.1f, 0.2f, 0.3f }, "extracted environment should match");
    check(data.directional_light_.has_value(), "extractor should include the directional light");
    if (data.directional_light_) {
        check_near(data.directional_light_->intensity_, 3.0f, "extracted light intensity should match");
    }

    camera.projection_type_ = mini::scene::CameraComponent::ProjectionType::Orthographic;
    camera.orthographic_left_ = -2.0f;
    camera.orthographic_right_ = 2.0f;
    camera.orthographic_bottom_ = -1.0f;
    camera.orthographic_top_ = 1.0f;
    camera.orthographic_near_ = 0.1f;
    camera.orthographic_far_ = 100.0f;
    const auto ortho = extractor.extract(scene);
    check_near(ortho.camera_.projection_[0][0], 0.5f, "extractor should build an orthographic projection");
    camera.projection_type_ = mini::scene::CameraComponent::ProjectionType::Perspective;

    mini::render::CameraData override_camera;
    override_camera.view_ = glm::mat4{ 2.0f };
    override_camera.projection_ = glm::mat4{ 3.0f };
    override_camera.position_ = glm::vec3{ 7.0f, 8.0f, 9.0f };

    mini::scene::SceneExtractor::Options options;
    options.camera_override_ = override_camera;
    const auto overridden = extractor.extract(scene, options);
    check_near(glm::vec3{ overridden.camera_.view_[3] }, glm::vec3{ 0.0f, 0.0f, 0.0f }, "camera override view should be used");
    check_near(glm::vec3{ overridden.camera_.view_[0] }, glm::vec3{ 2.0f, 0.0f, 0.0f }, "camera override should not use the scene camera");
    check_near(overridden.camera_.position_, override_camera.position_, "camera override position should be used");

    camera.primary_ = false;
    const auto fallback = extractor.extract(scene);
    check_near(fallback.camera_.position_, glm::vec3{ 0.0f, 0.0f, 5.0f }, "primary policy should fall back to the first camera");

    mini::scene::Scene empty;
    bool threw = false;
    try {
        extractor.extract(empty);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    check(threw, "extractor should throw when the scene has no camera");
}

} // namespace

int main() {
    test_uuid();
    test_scene_crud();
    test_transform_and_camera();
    test_scene_copy();
    test_scene_extractor();

    if (failures != 0) {
        std::cerr << failures << " check(s) failed\n";
        return 1;
    }

    std::cout << "All scene tests passed\n";
    return 0;
}
