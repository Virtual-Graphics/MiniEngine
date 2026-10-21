#pragma once
#include "editor/project/project.h"
#include "base/logger.h"
namespace editor {

// temporary place for all functions. Eventually some of these might be moved to other classes
class ProjectManager {
public:
    static ProjectManager& instance(){
        static ProjectManager instance;
        return instance;
    }

    void open_project(const std::filesystem::path& project_file) {
        ProjectInfo info;
        if (!load_project_info(project_file, info)) {
            base::log::error("Failed to load project info from {} ", project_file.string());
            throw std::runtime_error("Failed to load project info from " + project_file.string());
        }
        current_project_dir_ = project_file;
        current_project_info_ = info;
    }

    void save_project(const std::filesystem::path& project_file) {
        if (!save_project_info(project_file, current_project_info_)) {
            base::log::error("Failed to save project info to {} ", project_file.string());
            throw std::runtime_error("Failed to save project info to " + project_file.string());
        }
        current_project_dir_ = project_file;
    }

    std::filesystem::path current_project_dir() const { return current_project_dir_.parent_path(); };
    const ProjectInfo&    current_project_info() const { return current_project_info_; };
private:
    ProjectManager() = default;
    std::filesystem::path current_project_dir_;
    ProjectInfo current_project_info_;
};

}
