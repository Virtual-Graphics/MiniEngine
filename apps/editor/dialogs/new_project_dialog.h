#pragma once
#include "editor/dialogs/dialog.h"
#include <filesystem>
#include <functional>

namespace editor {
class NewProjectDialog : public Dialog {
public:
    using Create = std::function<void(const std::filesystem::path&, const std::string&, const std::string&, const std::filesystem::path&)>;
    explicit NewProjectDialog(Create create) : Dialog("New Project"), create_(std::move(create)) {}
protected:
    void draw_content() override;
private:
    Create create_;
    std::string name_, author_, assets_ = "assets", error_;
    std::filesystem::path directory_;
};
} // namespace editor
