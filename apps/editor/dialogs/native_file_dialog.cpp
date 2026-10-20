#include "editor/dialogs/native_file_dialog.h"
#include "editor/project/project.h"
#include <memory>
#include <nfd.h>
#include <stdexcept>

namespace editor {
namespace {
std::optional<std::filesystem::path> result(nfdresult_t status, nfdu8char_t* path) {
    const std::unique_ptr<nfdu8char_t, decltype(&NFD_FreePathU8)> owned(path, NFD_FreePathU8);
    if (status == NFD_CANCEL) return std::nullopt;
    if (status != NFD_OKAY) throw std::runtime_error(NFD_GetError() ? NFD_GetError() : "File dialog failed");
    return path_from_utf8(path);
}
} // namespace
NativeFileDialog::NativeFileDialog() {
    if (NFD_Init() != NFD_OKAY) throw std::runtime_error(NFD_GetError() ? NFD_GetError() : "File dialog initialization failed");
}
NativeFileDialog::~NativeFileDialog() { NFD_Quit(); }

std::optional<std::filesystem::path> NativeFileDialog::open(const char* description, const char* extension,
        const std::filesystem::path& directory) {
    const nfdu8filteritem_t filter{ description, extension };
    const auto initial = path_to_utf8(directory);
    nfdu8char_t* path = nullptr;
    const auto status = NFD_OpenDialogU8(&path, &filter, 1, initial.empty() ? nullptr : initial.c_str());
    return result(status, path);
}
std::optional<std::filesystem::path> NativeFileDialog::save_scene(const std::filesystem::path& directory,
        const std::filesystem::path& filename) {
    const nfdu8filteritem_t filter{ "Vulcal Scene", "vscene" };
    const auto initial = path_to_utf8(directory);
    const auto name = path_to_utf8(filename);
    nfdu8char_t* path = nullptr;
    const auto status = NFD_SaveDialogU8(&path, &filter, 1, initial.c_str(), name.c_str());
    // Keep the exact filename checked by the native overwrite prompt.
    // Native backends supply the default extension; ProjectSession validates it.
    return result(status, path);
}
std::optional<std::filesystem::path> NativeFileDialog::pick_folder(const std::filesystem::path& directory) {
    const auto initial = path_to_utf8(directory);
    nfdu8char_t* path = nullptr;
    const auto status = NFD_PickFolderU8(&path, initial.empty() ? nullptr : initial.c_str());
    return result(status, path);
}
} // namespace editor
