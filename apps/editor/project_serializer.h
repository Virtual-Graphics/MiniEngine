#pragma once
#include "editor/project.h"

namespace editor {
class ProjectSerializer {
public:
    static Project create(const std::filesystem::path& directory, const std::string& name,
            const std::string& author = {}, const std::filesystem::path& asset_directory = "assets");
    static Project load(const std::filesystem::path& path);
    static void save(const Project& project);
};
} // namespace editor
