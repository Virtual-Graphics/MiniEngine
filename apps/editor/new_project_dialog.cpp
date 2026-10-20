#include "editor/new_project_dialog.h"
#include "editor/native_file_dialog.h"
#include "editor/project.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace editor {
void NewProjectDialog::draw_content() {
    ImGui::SetNextItemWidth(360);
    ImGui::InputText("Project name", &name_);
    ImGui::TextUnformatted("Project directory");
    ImGui::TextWrapped("%s", directory_.empty() ? "Choose an existing directory" : path_to_utf8(directory_).c_str());
    if (ImGui::Button("Browse...")) {
        try {
            if (auto directory = NativeFileDialog::pick_folder(directory_)) directory_ = *directory;
        } catch (const std::exception& error) { error_ = error.what(); }
    }
    ImGui::SetNextItemWidth(360);
    ImGui::InputText("Author (optional)", &author_);
    ImGui::SetNextItemWidth(360);
    ImGui::InputText("Asset directory", &assets_);
    ImGui::TextDisabled("Paths are relative to the project directory.");
    if (!error_.empty()) {
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 440);
        ImGui::TextColored({ 1, 0.4f, 0.3f, 1 }, "%s", error_.c_str());
        ImGui::PopTextWrapPos();
    }
    ImGui::BeginDisabled(name_.empty() || directory_.empty() || assets_.empty());
    if (ImGui::Button("Create project")) {
        try {
            create_(directory_, name_, author_, path_from_utf8(assets_));
            error_.clear();
            close();
        } catch (const std::exception& error) { error_ = error.what(); }
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) { error_.clear(); close(); }
}
} // namespace editor
