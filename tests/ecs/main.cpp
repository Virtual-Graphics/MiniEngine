#include "base/uuid.h"
#include "scene/components.h"
#include "scene/scene.h"
#include "scene/scene_extractor.h"

#include <cmath>
#include <cstddef>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace {

void expect_near(const glm::vec3& actual, const glm::vec3& expected, float epsilon = 1e-4f) {
    EXPECT_NEAR(actual.x, expected.x, epsilon);
    EXPECT_NEAR(actual.y, expected.y, epsilon);
    EXPECT_NEAR(actual.z, expected.z, epsilon);
}

mini::scene::Entity find_entity_by_id(mini::scene::Scene& scene, const base::UUID& id) {
    for (const auto entity: scene.registry().view<mini::scene::IDComponent>()) {
        if (scene.registry().get<mini::scene::IDComponent>(entity).id_ == id) {
            return mini::scene::Entity{ entity, scene };
        }
    }

    return {};
}

TEST(Uuid, DefaultIsNil) {
    const base::UUID nil;
    EXPECT_TRUE(nil.is_nil());
    EXPECT_EQ(nil.value(), 0ull);
}

TEST(Uuid, GeneratedUuidsAreUnique) {
    const auto first = base::UUID::generate();
    const auto second = base::UUID::generate();
    EXPECT_FALSE(first.is_nil());
    EXPECT_NE(first, second);
}

TEST(Uuid, ToStringIsZeroPaddedHex) {
    const auto uuid = base::UUID::generate();
    EXPECT_EQ(uuid.to_string().size(), static_cast<std::size_t>(16));
    EXPECT_EQ(base::UUID{ 0x1234ull }.to_string(), std::string{ "0000000000001234" });
}

TEST(Uuid, UsableAsUnorderedMapKey) {
    const auto first = base::UUID::generate();
    const auto second = base::UUID::generate();

    std::unordered_map<base::UUID, int> values;
    values.emplace(first, 1);
    values.emplace(second, 2);
    EXPECT_EQ(values.size(), static_cast<std::size_t>(2));
}

TEST(Scene, CreatesEntityWithDefaultComponents) {
    mini::scene::Scene scene;
    auto entity = scene.create_entity("Player");

    EXPECT_TRUE(static_cast<bool>(entity));
    EXPECT_TRUE(entity.has_component<mini::scene::IDComponent>());
    EXPECT_TRUE(entity.has_component<mini::scene::TagComponent>());
    EXPECT_TRUE(entity.has_component<mini::scene::TransformComponent>());
    EXPECT_EQ(entity.get_component<mini::scene::TagComponent>().name_, std::string{ "Player" });
}

TEST(Scene, AddGetAndRemoveComponents) {
    mini::scene::Scene scene;
    auto entity = scene.create_entity("Player");

    auto& mesh_renderer = entity.add_component<mini::scene::MeshRendererComponent>();
    mesh_renderer.mesh_ = mini::render::MeshHandle{ 7 };
    mesh_renderer.material_ = mini::render::MaterialHandle{ 9 };
    EXPECT_TRUE(entity.has_component<mini::scene::MeshRendererComponent>());
    EXPECT_EQ(entity.get_component<mini::scene::MeshRendererComponent>().mesh_.value_, 7ull);

    entity.remove_component<mini::scene::MeshRendererComponent>();
    EXPECT_FALSE(entity.has_component<mini::scene::MeshRendererComponent>());
}

TEST(Scene, EntityIdsAreUnique) {
    mini::scene::Scene scene;
    auto first = scene.create_entity("Player");
    auto second = scene.create_entity("Enemy");

    EXPECT_NE(first.get_component<mini::scene::IDComponent>().id_, second.get_component<mini::scene::IDComponent>().id_);
}

TEST(Scene, SceneLevelComponentAccess) {
    mini::scene::Scene scene;
    auto entity = scene.create_entity("Player");

    EXPECT_TRUE(scene.has_component<mini::scene::IDComponent>(entity));

    scene.get_component<mini::scene::TagComponent>(entity).name_ = "Boss";
    EXPECT_EQ(scene.get_component<mini::scene::TagComponent>(entity).name_, std::string{ "Boss" });
}

TEST(Scene, RejectsForeignEntityEvenWhenItsHandleMatches) {
    mini::scene::Scene scene;
    mini::scene::Scene other;
    auto local = scene.create_entity("Local");
    auto foreign = other.create_entity("Foreign");
    EXPECT_THROW(scene.destroy_entity(foreign), std::invalid_argument);
    EXPECT_TRUE(local.valid());
    EXPECT_TRUE(foreign.valid());
}

TEST(Scene, RequiredComponentsCannotBeRemovedThroughEitherApi) {
    mini::scene::Scene scene;
    auto entity = scene.create_entity();
    EXPECT_THROW(entity.remove_component<mini::scene::IDComponent>(), std::logic_error);
    EXPECT_THROW(scene.remove_component<mini::scene::TagComponent>(entity), std::logic_error);
    EXPECT_THROW(entity.remove_component<mini::scene::TransformComponent>(), std::logic_error);
    EXPECT_TRUE(entity.has_component<mini::scene::IDComponent>());
    EXPECT_TRUE(entity.has_component<mini::scene::TagComponent>());
    EXPECT_TRUE(entity.has_component<mini::scene::TransformComponent>());
}

TEST(Scene, WrapperTraversalFiltersComponentsAndReturnsOwnedEntities) {
    mini::scene::Scene scene;
    auto entity = scene.create_entity("Renderable");
    entity.add_component<mini::scene::MeshRendererComponent>();
    scene.create_entity("Empty");
    int visited = 0;
    scene.each<mini::scene::TagComponent, mini::scene::MeshRendererComponent>([&](mini::scene::Entity current, auto& tag, auto& mesh) {
        EXPECT_EQ(current, entity);
        EXPECT_TRUE(scene.contains(current));
        tag.name_ = "Edited";
        mesh.visible_ = false;
        ++visited;
    });
    EXPECT_EQ(visited, 1);
    EXPECT_EQ(entity.get_component<mini::scene::TagComponent>().name_, "Edited");
    EXPECT_FALSE(entity.get_component<mini::scene::MeshRendererComponent>().visible_);
}

TEST(Scene, DuplicateOrInvalidComponentAccessFailsBeforeRegistryMutation) {
    mini::scene::Scene scene;
    auto entity = scene.create_entity();
    entity.add_component<mini::scene::CameraComponent>();
    EXPECT_THROW(entity.add_component<mini::scene::CameraComponent>(), std::logic_error);
    EXPECT_THROW(entity.get_component<mini::scene::LightComponent>(), std::out_of_range);
    scene.destroy_entity(entity);
    EXPECT_FALSE(scene.contains(entity));
    EXPECT_THROW(entity.add_component<mini::scene::LightComponent>(), std::invalid_argument);
}

TEST(Scene, DestroyEntityInvalidatesOnlyThatEntity) {
    mini::scene::Scene scene;
    auto entity = scene.create_entity("Player");
    auto second = scene.create_entity("Enemy");

    scene.destroy_entity(second);
    EXPECT_FALSE(second.valid());
    EXPECT_FALSE(static_cast<bool>(second));
    EXPECT_TRUE(entity.valid());
}

TEST(TransformAndCamera, TransformMatrixAppliesTranslationAndScale) {
    mini::scene::TransformComponent transform;
    transform.translation_ = glm::vec3{ 1.0f, 2.0f, 3.0f };
    transform.scale_ = glm::vec3{ 2.0f };

    const auto matrix = transform.matrix();
    expect_near(glm::vec3{ matrix[3] }, glm::vec3{ 1.0f, 2.0f, 3.0f });
    expect_near(glm::vec3{ matrix * glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f } }, glm::vec3{ 3.0f, 2.0f, 3.0f });
}

TEST(TransformAndCamera, SetAspectUpdatesPerspectiveAspect) {
    mini::scene::CameraComponent camera;
    camera.perspective_aspect_ = 1.5f;

    camera.set_aspect(2.0f);
    EXPECT_FLOAT_EQ(camera.perspective_aspect_, 2.0f);
}

TEST(TransformAndCamera, TransformRoundTripsCameraOrientation) {
    const glm::mat4 camera_world =
            glm::inverse(glm::lookAt(glm::vec3{ 0.0f, 4.0f, 8.0f }, glm::vec3{ 0.0f, -0.5f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }));
    mini::scene::TransformComponent camera_transform;
    camera_transform.translation_ = glm::vec3{ camera_world[3] };
    camera_transform.rotation_ = glm::eulerAngles(glm::quat_cast(camera_world));

    const glm::mat3 expected_basis = glm::mat3{ camera_world };
    const glm::mat3 actual_basis = glm::mat3{ camera_transform.matrix() };
    expect_near(glm::vec3{ actual_basis[0] }, glm::vec3{ expected_basis[0] });
    expect_near(glm::vec3{ actual_basis[1] }, glm::vec3{ expected_basis[1] });
    expect_near(glm::vec3{ actual_basis[2] }, glm::vec3{ expected_basis[2] });
}

TEST(SceneCopy, CopyPreservesEntitiesAndComponents) {
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
    environment.ambient_color_ = glm::vec3{ 0.2f, 0.3f, 0.4f };
    environment.clear_color_ = glm::vec4{ 0.5f, 0.6f, 0.7f, 1.0f };

    auto light_entity = scene.create_entity("Light");
    auto& light = light_entity.add_component<mini::scene::LightComponent>();
    light.direction_ = glm::vec3{ -1.0f };
    light.color_ = glm::vec3{ 1.0f };
    light.intensity_ = 2.0f;
    light.shadow_enabled_ = false;
    light.shadow_map_size_ = 1024;
    light.shadow_extent_ = 6.0f;

    const auto source_id = source.get_component<mini::scene::IDComponent>().id_;
    const auto source_translation = source.get_component<mini::scene::TransformComponent>().translation_;

    mini::scene::Scene copy = scene;
    auto copied_source = find_entity_by_id(copy, source_id);
    ASSERT_TRUE(copied_source.valid());
    EXPECT_EQ(copied_source.get_component<mini::scene::IDComponent>().id_, source_id);
    EXPECT_EQ(copied_source.get_component<mini::scene::TagComponent>().name_, std::string{ "Source" });
    expect_near(copied_source.get_component<mini::scene::TransformComponent>().translation_, source_translation);
    EXPECT_TRUE(copied_source.has_component<mini::scene::CameraComponent>());
    EXPECT_TRUE(copied_source.get_component<mini::scene::CameraComponent>().primary_);
    EXPECT_EQ(copied_source.get_component<mini::scene::MeshRendererComponent>().mesh_.value_, 21ull);

    const auto environment_view = copy.registry().view<const mini::scene::EnvironmentComponent>();
    ASSERT_FALSE(environment_view.empty());
    const auto& copied_environment = environment_view.get<const mini::scene::EnvironmentComponent>(environment_view.front());
    expect_near(copied_environment.ambient_color_, glm::vec3{ 0.2f, 0.3f, 0.4f });
    expect_near(glm::vec3{ copied_environment.clear_color_ }, glm::vec3{ 0.5f, 0.6f, 0.7f });

    const auto light_view = copy.registry().view<const mini::scene::LightComponent>();
    ASSERT_FALSE(light_view.empty());
    const auto& copied_light = light_view.get<const mini::scene::LightComponent>(light_view.front());
    expect_near(glm::vec3{ copied_light.direction_ }, glm::vec3{ -1.0f });
    EXPECT_FLOAT_EQ(copied_light.intensity_, 2.0f);
    EXPECT_FALSE(copied_light.shadow_enabled_);
    EXPECT_EQ(copied_light.shadow_map_size_, 1024u);
}

TEST(SceneCopy, CopiesAreIndependent) {
    mini::scene::Scene scene;
    auto source = scene.create_entity("Source");
    source.get_component<mini::scene::TransformComponent>().translation_ = glm::vec3{ 4.0f, 5.0f, 6.0f };
    const auto source_id = source.get_component<mini::scene::IDComponent>().id_;

    mini::scene::Scene copy = scene;
    auto copied_source = find_entity_by_id(copy, source_id);
    ASSERT_TRUE(copied_source.valid());

    copied_source.get_component<mini::scene::TransformComponent>().translation_.x = 100.0f;
    EXPECT_FLOAT_EQ(source.get_component<mini::scene::TransformComponent>().translation_.x, 4.0f);

    copy.destroy_entity(copied_source);
    EXPECT_FALSE(copied_source.valid());
    EXPECT_TRUE(source.valid());
}

TEST(SceneCopy, CopyAssignmentPreservesTransforms) {
    mini::scene::Scene scene;
    auto source = scene.create_entity("Source");
    source.get_component<mini::scene::TransformComponent>().translation_ = glm::vec3{ 4.0f, 5.0f, 6.0f };
    const auto source_id = source.get_component<mini::scene::IDComponent>().id_;
    const auto source_translation = source.get_component<mini::scene::TransformComponent>().translation_;

    mini::scene::Scene assigned;
    assigned = scene;

    auto assigned_source = find_entity_by_id(assigned, source_id);
    ASSERT_TRUE(assigned_source.valid());
    expect_near(assigned_source.get_component<mini::scene::TransformComponent>().translation_, source_translation);
}

class SceneExtractorTest : public ::testing::Test {
protected:
    void SetUp() override {
        camera_entity_ = scene_.create_entity("Camera");
        camera_entity_.get_component<mini::scene::TransformComponent>().translation_ = glm::vec3{ 0.0f, 0.0f, 5.0f };
        camera_ = &camera_entity_.add_component<mini::scene::CameraComponent>();
        camera_->primary_ = true;
        camera_->perspective_fov_y_ = glm::radians(45.0f);
        camera_->perspective_aspect_ = 2.0f;
        camera_->perspective_near_ = 0.1f;
        camera_->perspective_far_ = 100.0f;

        auto environment_entity = scene_.create_entity("Environment");
        auto& environment = environment_entity.add_component<mini::scene::EnvironmentComponent>();
        environment.ambient_color_ = glm::vec3{ 0.1f, 0.2f, 0.3f };
        environment.clear_color_ = glm::vec4{ 0.4f, 0.5f, 0.6f, 1.0f };

        auto light_entity = scene_.create_entity("Light");
        auto& light = light_entity.add_component<mini::scene::LightComponent>();
        light.direction_ = glm::vec3{ -1.0f };
        light.color_ = glm::vec3{ 1.0f };
        light.intensity_ = 3.0f;
        light.shadow_enabled_ = true;
        light.shadow_map_size_ = 1024;
        light.shadow_center_ = glm::vec3{ 0.0f, -0.5f, 0.0f };
        light.shadow_extent_ = 6.0f;
        light.shadow_near_ = 0.2f;
        light.shadow_far_ = 30.0f;
        light.shadow_depth_bias_ = 0.002f;
        light.shadow_normal_bias_ = 0.03f;

        auto mesh_entity = scene_.create_entity("Mesh");
        mesh_entity.get_component<mini::scene::TransformComponent>().translation_ = glm::vec3{ 1.0f, 2.0f, 3.0f };
        auto& mesh_renderer = mesh_entity.add_component<mini::scene::MeshRendererComponent>();
        mesh_renderer.mesh_ = mini::render::MeshHandle{ 11 };
        mesh_renderer.material_ = mini::render::MaterialHandle{ 22 };

        auto hidden_entity = scene_.create_entity("Hidden");
        auto& hidden_renderer = hidden_entity.add_component<mini::scene::MeshRendererComponent>();
        hidden_renderer.mesh_ = mini::render::MeshHandle{ 33 };
        hidden_renderer.material_ = mini::render::MaterialHandle{ 44 };
        hidden_renderer.visible_ = false;
    }

    mini::scene::Scene scene_;
    mini::scene::Entity camera_entity_;
    mini::scene::CameraComponent* camera_ = nullptr;
    mini::scene::SceneExtractor extractor_;
};

TEST_F(SceneExtractorTest, UsesPrimaryCamera) {
    const auto data = extractor_.extract(scene_);

    expect_near(data.camera_.position_, glm::vec3{ 0.0f, 0.0f, 5.0f });
    expect_near(glm::vec3{ data.camera_.view_[3] }, glm::vec3{ 0.0f, 0.0f, -5.0f });

    const float expected_projection_00 = 1.0f / (camera_->perspective_aspect_ * std::tan(camera_->perspective_fov_y_ * 0.5f));
    EXPECT_NEAR(data.camera_.projection_[0][0], expected_projection_00, 1e-4f);
}

TEST_F(SceneExtractorTest, SkipsInvisibleRenderers) {
    const auto data = extractor_.extract(scene_);

    ASSERT_EQ(data.items_.size(), static_cast<std::size_t>(1));
    EXPECT_EQ(data.items_[0].mesh_.value_, 11ull);
    EXPECT_EQ(data.items_[0].material_.value_, 22ull);
    expect_near(glm::vec3{ data.items_[0].world_transform_[3] }, glm::vec3{ 1.0f, 2.0f, 3.0f });
}

TEST_F(SceneExtractorTest, ExtractsEnvironment) {
    const auto data = extractor_.extract(scene_);

    expect_near(data.environment_.ambient_color_, glm::vec3{ 0.1f, 0.2f, 0.3f });
    expect_near(glm::vec3{ data.environment_.clear_color_ }, glm::vec3{ 0.4f, 0.5f, 0.6f });
}

TEST_F(SceneExtractorTest, ExtractsDirectionalLightAndShadow) {
    const auto data = extractor_.extract(scene_);

    ASSERT_TRUE(data.directional_light_.has_value());
    expect_near(glm::vec3{ data.directional_light_->direction_ }, glm::vec3{ -1.0f });
    expect_near(glm::vec3{ data.directional_light_->color_ }, glm::vec3{ 1.0f });
    EXPECT_FLOAT_EQ(data.directional_light_->intensity_, 3.0f);
    EXPECT_TRUE(data.directional_light_->shadow_.enabled_);
    EXPECT_EQ(data.directional_light_->shadow_.map_size_, 1024u);
    expect_near(glm::vec3{ data.directional_light_->shadow_.center_ }, glm::vec3{ 0.0f, -0.5f, 0.0f });
    EXPECT_FLOAT_EQ(data.directional_light_->shadow_.extent_, 6.0f);
    EXPECT_FLOAT_EQ(data.directional_light_->shadow_.near_, 0.2f);
    EXPECT_FLOAT_EQ(data.directional_light_->shadow_.far_, 30.0f);
    EXPECT_FLOAT_EQ(data.directional_light_->shadow_.depth_bias_, 0.002f);
    EXPECT_FLOAT_EQ(data.directional_light_->shadow_.normal_bias_, 0.03f);
}

TEST_F(SceneExtractorTest, BuildsOrthographicProjection) {
    camera_->projection_type_ = mini::scene::CameraComponent::ProjectionType::Orthographic;
    camera_->orthographic_left_ = -2.0f;
    camera_->orthographic_right_ = 2.0f;
    camera_->orthographic_bottom_ = -1.0f;
    camera_->orthographic_top_ = 1.0f;
    camera_->orthographic_near_ = 0.1f;
    camera_->orthographic_far_ = 100.0f;

    const auto data = extractor_.extract(scene_);
    EXPECT_NEAR(data.camera_.projection_[0][0], 0.5f, 1e-4f);
}

TEST_F(SceneExtractorTest, CameraOverrideTakesPrecedence) {
    mini::render::CameraData override_camera;
    override_camera.view_ = glm::mat4{ 2.0f };
    override_camera.projection_ = glm::mat4{ 3.0f };
    override_camera.position_ = glm::vec3{ 7.0f, 8.0f, 9.0f };

    mini::scene::SceneExtractor::Options options;
    options.camera_override_ = override_camera;
    const auto data = extractor_.extract(scene_, options);

    expect_near(glm::vec3{ data.camera_.view_[3] }, glm::vec3{ 0.0f, 0.0f, 0.0f });
    expect_near(glm::vec3{ data.camera_.view_[0] }, glm::vec3{ 2.0f, 0.0f, 0.0f });
    expect_near(data.camera_.position_, override_camera.position_);
}

TEST_F(SceneExtractorTest, FallsBackToFirstCameraWhenNoneIsPrimary) {
    camera_->primary_ = false;

    const auto data = extractor_.extract(scene_);
    expect_near(data.camera_.position_, glm::vec3{ 0.0f, 0.0f, 5.0f });
}

TEST_F(SceneExtractorTest, ThrowsWhenSceneHasNoCamera) {
    mini::scene::Scene empty;
    EXPECT_THROW(extractor_.extract(empty), std::runtime_error);
}

} // namespace
