#include "editor/editor_context.h"

#include "base/logger.h"

#include <stdexcept>
#include <utility>

namespace editor {

vulcal::scene::Scene& EditorContext::scene() {
    if (!scene_) throw std::logic_error("No scene is open");
    return *scene_;
}

const vulcal::scene::Scene& EditorContext::scene() const {
    if (!scene_) throw std::logic_error("No scene is open");
    return *scene_;
}

void EditorContext::set_scene(std::unique_ptr<vulcal::scene::Scene> scene, std::filesystem::path path) {
    if (!scene) throw std::invalid_argument("EditorContext requires a scene");
    selected_ = {};
    scene_ = std::move(scene);
    scene_path_ = std::move(path);
}

void EditorContext::set_project(Project project, std::unique_ptr<vulcal::scene::Scene> scene, std::filesystem::path path) {
    set_scene(std::move(scene), std::move(path));
    project_ = std::move(project);
}

void EditorContext::update_saved_scene(Project project, std::filesystem::path path) {
    project_ = std::move(project);
    scene_path_ = std::move(path);
}

vulcal::scene::Entity EditorContext::selected_entity() const { return scene_ && scene_->contains(selected_) ? selected_ : vulcal::scene::Entity{}; }

void EditorContext::select(vulcal::scene::Entity entity) {
    if (entity && (!scene_ || !scene_->contains(entity))) throw std::invalid_argument("Entity belongs to a different scene");
    selected_ = entity ? entity : vulcal::scene::Entity{};
}

vulcal::scene::Entity EditorContext::create_entity() {
    auto entity = scene().create_entity();
    select(entity);
    return entity;
}

void EditorContext::destroy_entity(vulcal::scene::Entity entity) {
    if (!entity) return;
    if (!scene_ || !scene_->contains(entity)) {
        base::log::error("Cannot destroy an entity that belongs to a different scene");
        throw std::invalid_argument("Entity belongs to a different scene");
    }
    if (selected_ == entity) selected_ = {};
    scene_->destroy_entity(entity);
}

} // namespace editor
