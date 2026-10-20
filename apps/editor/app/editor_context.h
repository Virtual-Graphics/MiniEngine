#pragma once

#include "scene/scene.h"
#include "editor/project/project.h"

#include <memory>
#include <optional>

namespace editor {

// Shared document and selection state. Panels never own references to an old scene.
class EditorContext {
public:
    bool has_scene() const { return scene_ != nullptr; }
    vulcal::scene::Scene& scene();
    const vulcal::scene::Scene& scene() const;
    void set_scene(std::unique_ptr<vulcal::scene::Scene> scene, std::filesystem::path path = {});
    const Project* project() const { return project_ ? &*project_ : nullptr; }
    void set_project(Project project, std::unique_ptr<vulcal::scene::Scene> scene, std::filesystem::path path = {});
    void update_saved_scene(Project project, std::filesystem::path path);
    const std::filesystem::path& scene_path() const { return scene_path_; }

    vulcal::scene::Entity selected_entity() const;
    void select(vulcal::scene::Entity entity);
    vulcal::scene::Entity create_entity();
    void destroy_entity(vulcal::scene::Entity entity);

private:
    std::optional<Project> project_;
    std::unique_ptr<vulcal::scene::Scene> scene_;
    std::filesystem::path scene_path_;
    vulcal::scene::Entity selected_;
};

} // namespace editor
