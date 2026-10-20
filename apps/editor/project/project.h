#pragma once

#include <filesystem>
#include <string>

namespace editor {

std::string path_to_utf8(const std::filesystem::path& path);
std::filesystem::path path_from_utf8(const std::string& text);
void validate_relative_path(const std::filesystem::path& path, bool allow_empty = false);

struct Project {
    std::string name_;
    std::string author_;
    std::filesystem::path asset_directory_{ "assets" };
    std::filesystem::path last_scene_;
    // Editor-session location; never serialized.
    std::filesystem::path file_path_;

    std::filesystem::path root_directory() const { return file_path_.parent_path(); }
    std::filesystem::path resolve(const std::filesystem::path& relative) const { return root_directory() / relative; }
    // Accepts absolute or project-relative paths; rejects paths outside the Asset directory.
    std::filesystem::path relative_asset_path(const std::filesystem::path& path) const;
};

} // namespace editor
