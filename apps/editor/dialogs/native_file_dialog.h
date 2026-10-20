#pragma once
#include <filesystem>
#include <optional>

namespace editor {
class NativeFileDialog {
public:
    NativeFileDialog();
    ~NativeFileDialog();
    NativeFileDialog(const NativeFileDialog&) = delete;
    NativeFileDialog& operator=(const NativeFileDialog&) = delete;
    static std::optional<std::filesystem::path> open(const char* description, const char* extension,
        const std::filesystem::path& directory = {});
    static std::optional<std::filesystem::path> save_scene(const std::filesystem::path& directory,
        const std::filesystem::path& filename = "Untitled.vscene");
    static std::optional<std::filesystem::path> pick_folder(const std::filesystem::path& directory = {});
};
} // namespace editor
