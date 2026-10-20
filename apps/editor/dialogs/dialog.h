#pragma once
#include <string>

namespace editor {
class Dialog {
public:
    explicit Dialog(std::string title) : title_(std::move(title)) {}
    virtual ~Dialog() = default;
    void open() { pending_open_ = true; }
    void draw();
protected:
    virtual void draw_content() = 0;
    void close();
private:
    std::string title_;
    bool pending_open_ = false;
};

class MessageDialog : public Dialog {
public:
    MessageDialog() : Dialog("Operation failed") {}
    void show(std::string message) { message_ = std::move(message); open(); }
protected:
    void draw_content() override;
private:
    std::string message_;
};
} // namespace editor
