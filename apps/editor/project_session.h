#pragma once
#include "editor/editor_context.h"

namespace editor {
// Parsing and disk writes finish before editor state changes.
class ProjectSession {
public:
    explicit ProjectSession(EditorContext& context) : context_(context) {}
    void create_project(const std::filesystem::path& directory, const std::string& name,
        const std::string& author = {}, const std::filesystem::path& assets = "assets");
    void open_project(const std::filesystem::path& path);
    void new_scene();
    void open_scene(const std::filesystem::path& path);
    void save_scene(const std::filesystem::path& path = {});
private:
    Project require_project() const;
    EditorContext& context_;
};
} // namespace editor
