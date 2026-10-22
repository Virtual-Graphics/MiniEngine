#include "editor/editor_context.h"

#include "base/logger.h"

#include <stdexcept>
#include <utility>

namespace editor {

vulcal::scene::Scene& EditorContext::scene() {
    auto* scene = project_manager_.current_scene();
    if (!scene) throw std::logic_error("No scene is currently open");
    return *scene;
}

const vulcal::scene::Scene& EditorContext::scene() const {
    const auto* scene = project_manager_.current_scene();
    if (!scene) throw std::logic_error("No scene is currently open");
    return *scene;
}

void EditorContext::set_scene(std::unique_ptr<vulcal::scene::Scene> scene) {
    project_manager_.set_scene(std::move(scene));
    selected_ = {};
}

void EditorContext::open_scene(const std::filesystem::path& scene_file) {
    project_manager_.open_scene(scene_file);
    selected_ = {};
}

void EditorContext::save_scene() { project_manager_.save_scene(); }

void EditorContext::save_scene_as(const std::filesystem::path& scene_file) { project_manager_.save_as_scene(scene_file); }

vulcal::scene::Entity EditorContext::selected_entity() const {
    const auto* scene = project_manager_.current_scene();
    return scene && scene->contains(selected_) ? selected_ : vulcal::scene::Entity{};
}

void EditorContext::select(vulcal::scene::Entity entity) {
    if (entity) {
        const auto* scene = project_manager_.current_scene();
        if (!scene || !scene->contains(entity)) throw std::invalid_argument("Entity belongs to a different scene");
    }
    selected_ = entity ? entity : vulcal::scene::Entity{};
}

vulcal::scene::Entity EditorContext::create_entity() {
    auto entity = scene().create_entity();
    select(entity);
    return entity;
}

void EditorContext::destroy_entity(vulcal::scene::Entity entity) {
    if (!entity) return;
    if (!scene().contains(entity)) {
        base::log::error("Cannot destroy an entity that belongs to a different scene");
        throw std::invalid_argument("Entity belongs to a different scene");
    }
    if (selected_ == entity) selected_ = {};
    scene().destroy_entity(entity);
}

} // namespace editor
