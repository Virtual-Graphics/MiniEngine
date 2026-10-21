#include "asset/material_bin.h"
#include "asset/mesh_bin.h"
#include "common/asset/asset_database.h"
#include "common/asset/obj_loader.h"
#include "editor/app/editor_context.h"
#include "editor/project/project_session.h"
#include "editor/serialization/project_serializer.h"
#include "editor/serialization/scene_serializer.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <limits>

namespace {
namespace fs = std::filesystem;
using namespace vulcal::scene;

class Persistence : public ::testing::Test {
protected:
    void SetUp() override {
        root_ = fs::temp_directory_path() / ("vulcal-project-test-" + base::UUID::generate().to_string());
        fs::create_directories(root_);
        root_ = fs::canonical(root_);
    }
    void TearDown() override { fs::remove_all(root_); }
    fs::path root_;
    editor::EditorContext context_;
    editor::ProjectSession session_{ context_ };
};

TEST_F(Persistence, CreatesProjectButLeavesDefaultSceneUnsaved) {
    session_.create_project(root_, "Demo", "Artichoke", "content");
    ASSERT_TRUE(context_.project());
    EXPECT_EQ(context_.project()->file_path_, root_ / "Demo.vproj");
    EXPECT_TRUE(fs::is_directory(root_ / "content"));
    EXPECT_TRUE(context_.scene_path().empty());
    EXPECT_TRUE(context_.project()->last_scene_.empty());
    EXPECT_EQ(context_.scene().registry().view<IDComponent>().size(), 2u);
    EXPECT_EQ(context_.scene().registry().view<LightComponent>().size(), 1u);
    EXPECT_EQ(context_.scene().registry().view<CameraComponent>().size(), 1u);
    EXPECT_FALSE(fs::exists(root_ / "content/scenes"));
    const auto saved = editor::ProjectSerializer::load(root_ / "Demo.vproj");
    EXPECT_EQ(saved.name_, "Demo");
    EXPECT_EQ(saved.author_, "Artichoke");
    EXPECT_EQ(saved.asset_directory_, "content");
}

TEST_F(Persistence, RejectsAnyExistingProjectWithoutReplacingIt) {
    session_.create_project(root_, "First");
    EXPECT_THROW(session_.create_project(root_, "Second"), std::exception);
    EXPECT_FALSE(fs::exists(root_ / "Second.vproj"));
    EXPECT_EQ(editor::ProjectSerializer::load(root_ / "First.vproj").name_, "First");
    EXPECT_EQ(context_.project()->name_, "First");
}

TEST_F(Persistence, RejectsEmptyDirectoryAndEscapingAssetPaths) {
    EXPECT_THROW(session_.create_project({}, "Demo"), std::exception);
    EXPECT_THROW(session_.create_project(root_, "../Demo"), std::exception);
    EXPECT_THROW(session_.create_project(root_, "Demo", "", "../outside"), std::exception);
    EXPECT_THROW(session_.create_project(root_, "Demo", "", root_ / "assets"), std::exception);
    EXPECT_FALSE(fs::exists(root_ / "Demo.vproj"));
}

TEST_F(Persistence, SceneSaveOpenAndProjectRelocationPreserveRelativeReferences) {
    session_.create_project(root_, "Demo", "", "content");
    auto entity = context_.create_entity();
    entity.get_component<TagComponent>().name_ = "Mesh";
    const auto mesh_id = vulcal::asset::MeshID{ base::UUID::generate() };
    const auto material_id = vulcal::asset::MaterialID{ base::UUID::generate() };
    auto& renderer = entity.add_component<MeshRendererComponent>();
    renderer.mesh_ = mesh_id;
    renderer.material_ = material_id;
    const auto id = entity.get_component<IDComponent>().id_;
    session_.save_scene(root_ / "content/scenes/main.vscene");
    EXPECT_EQ(context_.project()->last_scene_, "content/scenes/main.vscene");
    const auto moved = root_ / "relocated";
    fs::create_directories(moved);
    fs::rename(root_ / "Demo.vproj", moved / "Demo.vproj");
    fs::rename(root_ / "content", moved / "content");
    session_.open_project(moved / "Demo.vproj");
    EXPECT_EQ(context_.scene_path(), moved / "content/scenes/main.vscene");
    bool found = false;
    context_.scene().each<IDComponent, MeshRendererComponent>([&](auto, const auto& identity, const auto& mesh) {
        EXPECT_EQ(identity.id_, id);
        EXPECT_EQ(mesh.mesh_, mesh_id);
        EXPECT_EQ(mesh.material_, material_id);
        found = true;
    });
    EXPECT_TRUE(found);
}

TEST_F(Persistence, MissingLastSceneFallsBackToUnsavedDefault) {
    session_.create_project(root_, "Demo");
    session_.save_scene(root_ / "assets/scenes/main.vscene");
    fs::remove(root_ / "assets/scenes/main.vscene");
    session_.open_project(root_ / "Demo.vproj");
    EXPECT_TRUE(context_.scene_path().empty());
    EXPECT_EQ(context_.scene().registry().view<IDComponent>().size(), 2u);
}

TEST_F(Persistence, FailedOpenAndSavePreserveScenePathAndSelection) {
    session_.create_project(root_, "Demo");
    session_.save_scene(root_ / "assets/scenes/main.vscene");
    auto selected = context_.create_entity();
    std::ofstream(root_ / "assets/broken.vscene") << "format_version: 400\nentities: []\n";
    EXPECT_THROW(session_.open_scene(root_ / "assets/broken.vscene"), std::exception);
    EXPECT_THROW(session_.save_scene(root_ / "outside.vscene"), std::exception);
    EXPECT_EQ(context_.selected_entity(), selected);
    EXPECT_EQ(context_.scene_path(), root_ / "assets/scenes/main.vscene");
    EXPECT_EQ(editor::ProjectSerializer::load(root_ / "Demo.vproj").last_scene_, "assets/scenes/main.vscene");
}

TEST_F(Persistence, RoundTripsEveryComponentField) {
    Scene scene;
    auto entity = scene.create_entity("All fields: 测试");
    entity.get_component<IDComponent>().id_ = base::UUID{ 0xfedcba9876543210ull };
    auto& transform = entity.get_component<TransformComponent>();
    transform.translation_ = { 1, 2, 3 };
    transform.rotation_ = { 0.1f, 0.2f, 0.3f };
    transform.scale_ = { 2, 3, 4 };
    auto& mesh = entity.add_component<MeshRendererComponent>();
    mesh.mesh_ = vulcal::asset::MeshID{ base::UUID{ 0x0123456789abcdefull } };
    mesh.material_ = vulcal::asset::MaterialID{ base::UUID{ 0x1111222233334444ull } };
    mesh.visible_ = false;
    auto& camera = entity.add_component<CameraComponent>();
    camera.projection_type_ = CameraComponent::ProjectionType::Orthographic;
    camera.primary_ = false;
    camera.perspective_fov_y_ = 0.9f;
    camera.perspective_aspect_ = 1.7f;
    camera.perspective_near_ = 0.2f;
    camera.perspective_far_ = 200;
    camera.orthographic_left_ = -4;
    camera.orthographic_right_ = 5;
    camera.orthographic_bottom_ = -6;
    camera.orthographic_top_ = 7;
    camera.orthographic_near_ = 0.3f;
    camera.orthographic_far_ = 90;
    auto& light = entity.add_component<LightComponent>();
    light.color_ = { 0.2f, 0.4f, 0.6f };
    light.intensity_ = 3;
    light.shadow_enabled_ = false;
    light.shadow_map_size_ = 1024;
    light.shadow_center_ = { 4, 5, 6 };
    light.shadow_extent_ = 12;
    light.shadow_near_ = 0.4f;
    light.shadow_far_ = 75;
    light.shadow_depth_bias_ = 0.003f;
    light.shadow_normal_bias_ = 0.04f;
    auto& environment = entity.add_component<EnvironmentComponent>();
    environment.ambient_color_ = { 0.3f, 0.2f, 0.1f };
    environment.clear_color_ = { 0.1f, 0.2f, 0.3f, 0.4f };
    editor::SceneSerializer::save(scene, root_ / "all.vscene");
    auto loaded = editor::SceneSerializer::load(root_ / "all.vscene");
    ASSERT_EQ(loaded->registry().view<IDComponent>().size(), 1u);
    const auto handle = loaded->registry().view<IDComponent>().front();
    Entity copy{ handle, *loaded };
    EXPECT_EQ(copy.get_component<IDComponent>().id_.value(), 0xfedcba9876543210ull);
    EXPECT_EQ(copy.get_component<TagComponent>().name_, "All fields: 测试");
    EXPECT_EQ(copy.get_component<TransformComponent>().translation_, transform.translation_);
    EXPECT_EQ(copy.get_component<TransformComponent>().rotation_, transform.rotation_);
    EXPECT_EQ(copy.get_component<TransformComponent>().scale_, transform.scale_);
    EXPECT_EQ(copy.get_component<MeshRendererComponent>().mesh_, mesh.mesh_);
    EXPECT_FALSE(copy.get_component<MeshRendererComponent>().visible_);
    EXPECT_EQ(copy.get_component<MeshRendererComponent>().material_, mesh.material_);
    const auto& c = copy.get_component<CameraComponent>();
    EXPECT_EQ(c.projection_type_, camera.projection_type_);
    EXPECT_EQ(c.primary_, camera.primary_);
    EXPECT_FLOAT_EQ(c.perspective_fov_y_, 0.9f);
    EXPECT_FLOAT_EQ(c.perspective_aspect_, 1.7f);
    EXPECT_FLOAT_EQ(c.perspective_near_, 0.2f);
    EXPECT_FLOAT_EQ(c.perspective_far_, 200);
    EXPECT_FLOAT_EQ(c.orthographic_left_, -4);
    EXPECT_FLOAT_EQ(c.orthographic_right_, 5);
    EXPECT_FLOAT_EQ(c.orthographic_bottom_, -6);
    EXPECT_FLOAT_EQ(c.orthographic_top_, 7);
    EXPECT_FLOAT_EQ(c.orthographic_near_, 0.3f);
    EXPECT_FLOAT_EQ(c.orthographic_far_, 90);
    const auto& l = copy.get_component<LightComponent>();
    EXPECT_EQ(l.type_, LightType::Directional);
    EXPECT_EQ(l.color_, light.color_);
    EXPECT_FLOAT_EQ(l.intensity_, 3);
    EXPECT_FALSE(l.shadow_enabled_);
    EXPECT_EQ(l.shadow_map_size_, 1024u);
    EXPECT_EQ(l.shadow_center_, light.shadow_center_);
    EXPECT_FLOAT_EQ(l.shadow_extent_, 12);
    EXPECT_FLOAT_EQ(l.shadow_near_, 0.4f);
    EXPECT_FLOAT_EQ(l.shadow_far_, 75);
    EXPECT_FLOAT_EQ(l.shadow_depth_bias_, 0.003f);
    EXPECT_FLOAT_EQ(l.shadow_normal_bias_, 0.04f);
    EXPECT_EQ(copy.get_component<EnvironmentComponent>().ambient_color_, environment.ambient_color_);
    EXPECT_EQ(copy.get_component<EnvironmentComponent>().clear_color_, environment.clear_color_);
}

TEST_F(Persistence, RejectsMalformedScenesAndDuplicateIds) {
    const auto path = root_ / "bad.vscene";
    std::ofstream(path) << "format_version: 1\nentities: broken\n";
    EXPECT_THROW(editor::SceneSerializer::load(path), std::exception);
    std::ofstream(path) << "format_version: 2\nentities:\n"
                           "  - {id: 1, name: First, transform: {translation: [0, 0, 0], rotation: [0, 0, 0], scale: [1, 1, 1]}}\n"
                           "  - {id: 1, name: Second, transform: {translation: [0, 0, 0], rotation: [0, 0, 0], scale: [1, 1, 1]}}\n";
    EXPECT_THROW(editor::SceneSerializer::load(path), std::exception);
    std::ofstream(path) << "format_version: 2\nentities:\n"
                           "  - {id: 1, name: Bad, transform: {translation: [0, 0, 0], rotation: [0, 0, 0], scale: [1, 1, 1]},"
                           " mesh_renderer: {mesh: not-an-id, material: '', visible: true}}\n";
    EXPECT_THROW(editor::SceneSerializer::load(path), std::exception);
}

TEST_F(Persistence, InvalidProjectionAndShadowSettingsPreserveActiveScene) {
    session_.create_project(root_, "Demo");
    const auto current = root_ / "assets/scenes/main.vscene";
    session_.save_scene(current);
    const auto selected = context_.create_entity();
    std::ifstream input(current);
    const std::string original{ std::istreambuf_iterator<char>{ input }, {} };
    const auto invalid = root_ / "assets/invalid.vscene";
    for (const auto& [key, value]: { std::pair{ "shadow_map_size", "0" }, { "shadow_map_size", "4294967295" }, { "shadow_map_size", "513" },
             { "shadow_extent", "0" }, { "shadow_near", "-1" }, { "shadow_far", "0.1" }, { "perspective_fov_y", "0" },
             { "perspective_fov_y", "3.1415928" }, { "perspective_aspect", "0" }, { "perspective_near", "0" }, { "perspective_far", "0.1" },
             { "orthographic_right", "-1" }, { "orthographic_top", "-1" }, { "orthographic_far", "0.1" } }) {
        SCOPED_TRACE(std::string(key) + ": " + value);
        auto text = original;
        const std::string field = std::string(key) + ": ";
        const auto position = text.find(field);
        ASSERT_NE(position, std::string::npos);
        const auto begin = position + field.size();
        text.replace(begin, text.find('\n', begin) - begin, value);
        std::ofstream(invalid) << text;
        EXPECT_THROW(session_.open_scene(invalid), std::exception);
        EXPECT_EQ(context_.selected_entity(), selected);
        EXPECT_EQ(context_.scene_path(), current);
        EXPECT_EQ(editor::ProjectSerializer::load(root_ / "Demo.vproj").last_scene_, "assets/scenes/main.vscene");
    }
}

TEST_F(Persistence, DoesNotWriteInvalidShadowSettings) {
    Scene scene;
    scene.create_entity("Invalid light").add_component<LightComponent>().shadow_map_size_ = 0;
    EXPECT_THROW(editor::SceneSerializer::save(scene, root_ / "invalid.vscene"), std::exception);
    EXPECT_FALSE(fs::exists(root_ / "invalid.vscene"));
}

TEST_F(Persistence, SceneOperationsRequireProjectAndNewSceneClearsCurrentPath) {
    EXPECT_THROW(session_.new_scene(), std::logic_error);
    EXPECT_THROW(session_.open_scene(root_ / "scene.vscene"), std::logic_error);
    EXPECT_THROW(session_.save_scene(root_ / "scene.vscene"), std::logic_error);
    session_.create_project(root_, "Demo");
    session_.save_scene(root_ / "assets/scenes/first.vscene");
    context_.create_entity();
    session_.new_scene();
    EXPECT_TRUE(context_.scene_path().empty());
    EXPECT_FALSE(context_.selected_entity());
    EXPECT_EQ(context_.scene().registry().view<IDComponent>().size(), 2u);
    session_.save_scene(root_ / "assets/scenes/second.vscene");
    session_.open_scene(root_ / "assets/scenes/first.vscene");
    EXPECT_EQ(editor::ProjectSerializer::load(root_ / "Demo.vproj").last_scene_, "assets/scenes/first.vscene");
    context_.create_entity();
    session_.save_scene();
    EXPECT_EQ(editor::SceneSerializer::load(context_.scene_path())->registry().view<IDComponent>().size(), 3u);
}

TEST_F(Persistence, FailedProjectSwitchAndMetadataWritePreserveCurrentState) {
    session_.create_project(root_, "Demo");
    session_.save_scene(root_ / "assets/scenes/main.vscene");
    auto selected = context_.create_entity();
    const auto other = root_ / "other";
    fs::create_directory(other);
    auto project = editor::ProjectSerializer::create(other, "Other");
    project.last_scene_ = "assets/broken.vscene";
    editor::ProjectSerializer::save(project);
    std::ofstream(other / project.last_scene_) << "invalid: scene\n";
    EXPECT_THROW(session_.open_project(project.file_path_), std::exception);
    EXPECT_EQ(context_.project()->name_, "Demo");
    EXPECT_EQ(context_.selected_entity(), selected);
    // A directory occupying the project file prevents atomic replacement.
    fs::rename(root_ / "Demo.vproj", root_ / "backup.vproj");
    fs::create_directory(root_ / "Demo.vproj");
    EXPECT_THROW(session_.save_scene(root_ / "assets/scenes/other.vscene"), std::exception);
    EXPECT_EQ(context_.scene_path(), root_ / "assets/scenes/main.vscene");
    EXPECT_EQ(context_.project()->last_scene_, "assets/scenes/main.vscene");
    EXPECT_EQ(context_.selected_entity(), selected);
}

TEST_F(Persistence, RejectsNonFiniteFields) {
    session_.create_project(root_, "Demo");
    auto entity = context_.create_entity();
    entity.add_component<MeshRendererComponent>();
    entity.get_component<TransformComponent>().translation_.x = std::numeric_limits<float>::infinity();
    EXPECT_THROW(session_.save_scene(root_ / "assets/main.vscene"), std::exception);
    EXPECT_FALSE(fs::exists(root_ / "assets/main.vscene"));
}

TEST_F(Persistence, UnicodeProjectSceneAndObjPathsWork) {
    const auto directory = root_ / editor::path_from_utf8("项目");
    fs::create_directory(directory);
    session_.create_project(directory, "测试", "作者", editor::path_from_utf8("资源"));
    const auto path = directory / editor::path_from_utf8("资源/场景.vscene");
    session_.save_scene(path);
    session_.open_project(directory / editor::path_from_utf8("测试.vproj"));
    EXPECT_EQ(context_.project()->author_, "作者");
    EXPECT_EQ(context_.project()->last_scene_, editor::path_from_utf8("资源/场景.vscene"));
    const auto mesh_path = directory / editor::path_from_utf8("资源/模型.obj");
    std::ofstream(mesh_path) << "v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n";
    std::string error;
    const auto data = vulcal::asset_pipeline::load_obj(mesh_path, error);
    ASSERT_TRUE(data.has_value()) << error;
    EXPECT_EQ(data->vertices_.size(), 3u);
    EXPECT_EQ(data->indices_.size(), 3u);
    EXPECT_EQ(data->vertices_[0].normal, (glm::vec3{ 0, 0, 1 }));
    // OBJ files may provide normals for only some corners.
    std::ofstream(mesh_path) << "v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf 1//1 2 3\n";
    ASSERT_TRUE(vulcal::asset_pipeline::load_obj(mesh_path, error).has_value()) << error;
    EXPECT_EQ(vulcal::asset_pipeline::load_obj(mesh_path, error)->indices_.size(), 3u);
}

TEST_F(Persistence, AssetDatabaseCreatesMetaAndMeshCache) {
    session_.create_project(root_, "Demo");
    const auto source = root_ / "assets/bunny.obj";
    std::ofstream(source) << "v 0 0 0\nv 1 0 0\nv 0 1 0\nvn 0 0 1\nf 1//1 2//1 3//1\n";

    vulcal::asset_pipeline::AssetDatabase assets;
    const auto& project = *context_.project();
    assets.set_roots(project.resolve(project.asset_directory_), project.root_directory() / ".vulcal_engine");
    ASSERT_EQ(assets.records().size(), 1u);
    const auto& record = assets.records().front();
    EXPECT_EQ(record.kind_, vulcal::asset_pipeline::AssetKind::Mesh);
    EXPECT_EQ(record.status_, vulcal::asset_pipeline::AssetStatus::Ready);
    EXPECT_TRUE(fs::exists(editor::path_from_utf8(editor::path_to_utf8(source) + ".meta")));
    EXPECT_TRUE(fs::exists(record.cache_path_));

    const auto data = vulcal::asset::decode_mesh_bin(record.cache_path_);
    ASSERT_TRUE(data.has_value());
    EXPECT_EQ(data->vertices_.size(), 3u);
    EXPECT_EQ(data->indices_.size(), 3u);

    // The documented .meta example quotes the decimal ID; parsing accepts it and keeps identity.
    const auto meta = editor::path_from_utf8(editor::path_to_utf8(source) + ".meta");
    const auto original_id = record.id_;
    {
        std::ofstream(meta) << "format_version: 1\nid: \"" << original_id.value() << "\"\n";
    }
    assets.rescan();
    ASSERT_EQ(assets.records().size(), 1u);
    EXPECT_EQ(assets.records().front().id_, original_id);

    // Caches are disposable: deleting .vulcal_engine regenerates them from the source.
    fs::remove_all(assets.cache_directory());
    assets.rescan();
    ASSERT_EQ(assets.records().size(), 1u);
    EXPECT_TRUE(fs::exists(assets.records().front().cache_path_));
}

TEST_F(Persistence, AssetDatabaseCreatesMaterialAndReportsDuplicateIds) {
    session_.create_project(root_, "Demo");
    vulcal::asset_pipeline::AssetDatabase assets;
    const auto& project = *context_.project();
    assets.set_roots(project.resolve(project.asset_directory_), project.root_directory() / ".vulcal_engine");

    base::UUID id;
    std::string error;
    ASSERT_TRUE(assets.create_material("White", id, error)) << error;
    ASSERT_NE(assets.find(id), nullptr);
    const auto* material = assets.find(id);
    ASSERT_NE(material, nullptr);
    EXPECT_EQ(material->status_, vulcal::asset_pipeline::AssetStatus::Ready);
    const auto cache = vulcal::asset::decode_material_bin(material->cache_path_);
    ASSERT_TRUE(cache.has_value());
    EXPECT_FLOAT_EQ(cache->shininess_, 8.0f);

    // Copying a source together with its .meta produces a duplicate identity.
    const auto source = root_ / "assets/White.vmat";
    const auto meta = editor::path_from_utf8(editor::path_to_utf8(source) + ".meta");
    fs::copy_file(source, root_ / "assets/Copy.vmat");
    fs::copy_file(meta, editor::path_from_utf8(editor::path_to_utf8(root_ / "assets/Copy.vmat") + ".meta"));
    assets.rescan();
    ASSERT_EQ(assets.records().size(), 2u);
    int failed = 0;
    for (const auto& record: assets.records()) failed += record.status_ == vulcal::asset_pipeline::AssetStatus::Failed ? 1 : 0;
    EXPECT_EQ(failed, 1);
}
} // namespace
