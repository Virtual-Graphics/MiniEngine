#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace apps::nfd {

struct FileFilter {
    std::string name;
    std::string extension;
};


// Returns std::nullopt when the user cancels the dialog.
std::optional<std::filesystem::path> open_file(const std::vector<FileFilter>& filters, const std::filesystem::path& default_path = {});
std::optional<std::filesystem::path> open_file(const FileFilter& filter, const std::filesystem::path& default_path = {});

std::optional<std::filesystem::path> save_file(const std::vector<FileFilter>& filters, const std::filesystem::path& default_path = {},
        const std::filesystem::path& default_name = {});
std::optional<std::filesystem::path> save_file(const FileFilter& filter, const std::filesystem::path& default_path = {},
        const std::filesystem::path& default_name = {});

std::optional<std::filesystem::path> pick_folder(const std::filesystem::path& default_path = {});

} // namespace apps::nfd
