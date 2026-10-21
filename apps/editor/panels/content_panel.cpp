#include "editor/panels/content_panel.h"

#include "common/asset/asset_index.h"
#include "editor/app/editor_context.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace editor {
using vulcal::asset_pipeline::AssetIndex;
using vulcal::asset_pipeline::AssetKind;
using vulcal::asset_pipeline::AssetStatus;

void ContentPanel::draw(EditorContext& context, AssetIndex& assets, bool& open) {
    if (!open) return;
    if (ImGui::Begin("Content", &open)) {
        if (!context.project()) {
            ImGui::TextDisabled("Open a project to browse assets.");
            ImGui::End();
            return;
        }

        if (ImGui::Button("Refresh")) {
            assets.rescan();
            error_.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Create Material")) {
            new_material_name_.clear();
            error_.clear();
            ImGui::OpenPopup("Create Material");
        }
        ImGui::SameLine();
        const auto selected = context.selected_asset();
        ImGui::BeginDisabled(selected.is_nil());
        if (ImGui::Button("Reimport")) {
            std::string error;
            if (assets.reimport(selected, error)) error_.clear();
            else
                error_ = std::move(error);
        }
        ImGui::EndDisabled();

        if (ImGui::BeginPopupModal("Create Material", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("Name", &new_material_name_);
            if (ImGui::Button("Create")) {
                base::UUID id;
                std::string error;
                if (assets.create_material(new_material_name_, id, error)) {
                    context.select_asset(id);
                    error_.clear();
                    ImGui::CloseCurrentPopup();
                } else {
                    error_ = std::move(error);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
            if (!error_.empty()) ImGui::TextWrapped("%s", error_.c_str());
            ImGui::EndPopup();
        }

        if (!error_.empty()) ImGui::TextWrapped("%s", error_.c_str());
        ImGui::Separator();

        ImGui::BeginChild("Assets");
        if (assets.records().empty()) ImGui::TextDisabled("No supported assets found. Add .obj or .vmat files and refresh.");
        for (std::size_t i = 0; i < assets.records().size(); ++i) {
            const auto& record = assets.records()[i];
            ImGui::PushID(static_cast<int>(i));
            std::string label = record.name_;
            label += record.kind_ == AssetKind::Mesh ? "  [Mesh]" : "  [Material]";
            if (record.status_ != AssetStatus::Ready) label += "  (failed)";
            if (ImGui::Selectable(label.c_str(), record.id_ == selected)) context.select_asset(record.id_);
            if (record.status_ != AssetStatus::Ready && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", record.error_.c_str());
            ImGui::PopID();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

} // namespace editor
