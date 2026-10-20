#include "editor/project_session.h"
#include "editor/default_scene.h"
#include "editor/project_serializer.h"
#include "editor/scene_serializer.h"

namespace editor {
namespace fs = std::filesystem;
namespace {
fs::path scene_reference(const Project& project, const fs::path& path) {
    if (path.extension() != ".vscene") throw std::invalid_argument("Select a .vscene file");
    return project.relative_asset_path(path);
}

void validate_meshes(const vulcal::scene::Scene& scene, const Project& project) {
    const auto view = scene.registry().view<const vulcal::scene::MeshRendererComponent>();
    for (const auto entity : view) {
        const auto& mesh = view.get<const vulcal::scene::MeshRendererComponent>(entity).mesh_;
        if (!mesh.empty()) project.relative_asset_path(mesh);
    }
}
} // namespace

Project ProjectSession::require_project() const {
    if (!context_.project()) throw std::logic_error("Open or create a project first");
    return *context_.project();
}

void ProjectSession::create_project(const fs::path& directory, const std::string& name,
        const std::string& author, const fs::path& assets) {
    auto scene = create_default_scene();
    auto project = ProjectSerializer::create(directory, name, author, assets);
    context_.set_project(std::move(project), std::move(scene));
}

void ProjectSession::open_project(const fs::path& path) {
    auto project = ProjectSerializer::load(path);
    std::unique_ptr<vulcal::scene::Scene> scene;
    fs::path scene_path;
    if (!project.last_scene_.empty() && fs::exists(project.resolve(project.last_scene_))) {
        scene_path = project.resolve(project.last_scene_);
        scene = SceneSerializer::load(scene_path);
        validate_meshes(*scene, project);
    } else scene = create_default_scene();
    context_.set_project(std::move(project), std::move(scene), std::move(scene_path));
}

void ProjectSession::new_scene() {
    require_project();
    context_.set_scene(create_default_scene());
}

void ProjectSession::open_scene(const fs::path& path) {
    auto project = require_project();
    project.last_scene_ = scene_reference(project, path);
    auto absolute = project.resolve(project.last_scene_);
    auto scene = SceneSerializer::load(absolute);
    validate_meshes(*scene, project);
    ProjectSerializer::save(project);
    context_.set_project(std::move(project), std::move(scene), std::move(absolute));
}

void ProjectSession::save_scene(const fs::path& path) {
    auto project = require_project();
    project.last_scene_ = scene_reference(project, path.empty() ? context_.scene_path() : path);
    auto absolute = project.resolve(project.last_scene_);
    validate_meshes(context_.scene(), project);
    fs::create_directories(absolute.parent_path());
    SceneSerializer::save(context_.scene(), absolute);
    ProjectSerializer::save(project);
    context_.update_saved_scene(std::move(project), std::move(absolute));
}
} // namespace editor
