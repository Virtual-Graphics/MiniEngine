#pragma once

#include "editor/project/project_manager.h"
#include "scene/scene.h"

#include <filesystem>
#include <memory>

namespace editor {

// Shared document and selection state. The current scene is owned by
// ProjectManager; panels never keep references to an old scene.
class EditorContext {
public:
    bool has_scene() const { return project_manager_.has_scene(); }

    vulcal::scene::Scene& scene();
    const vulcal::scene::Scene& scene() const;

    void set_scene(std::unique_ptr<vulcal::scene::Scene> scene);
    void open_scene(const std::filesystem::path& scene_file);
    void save_scene();
    void save_scene_as(const std::filesystem::path& scene_file);

    ProjectManager& project_manager() { return project_manager_; }
    const ProjectManager& project_manager() const { return project_manager_; }

    vulcal::scene::Entity selected_entity() const;
    void select(vulcal::scene::Entity entity);
    vulcal::scene::Entity create_entity();
    void destroy_entity(vulcal::scene::Entity entity);

private:
    ProjectManager& project_manager_ = ProjectManager::instance();
    vulcal::scene::Entity selected_;
};

} // namespace editor
