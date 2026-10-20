#include "editor/dialog.h"
#include <imgui.h>

namespace editor {
void Dialog::draw() {
    if (pending_open_) { ImGui::OpenPopup(title_.c_str()); pending_open_ = false; }
    const auto center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, { 0.5f, 0.5f });
    bool visible = true;
    if (ImGui::BeginPopupModal(title_.c_str(), &visible, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_content();
        ImGui::EndPopup();
    }
}
void Dialog::close() { ImGui::CloseCurrentPopup(); }
void MessageDialog::draw_content() {
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 480.0f);
    ImGui::TextUnformatted(message_.c_str());
    ImGui::PopTextWrapPos();
    if (ImGui::Button("OK")) close();
}
} // namespace editor
