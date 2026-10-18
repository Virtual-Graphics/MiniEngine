#pragma once

struct ImGuiContext;

namespace base {
class Window;
}

namespace apps {

// Owns one application's ImGui context and its SDL3/OpenGL backends.
// The window and its current OpenGL context must outlive this object.
// Owns the window's single event callback until destruction.
class ImGuiIntegration {
public:
    explicit ImGuiIntegration(base::Window& window);
    ~ImGuiIntegration();

    ImGuiIntegration(const ImGuiIntegration&) = delete;
    ImGuiIntegration& operator=(const ImGuiIntegration&) = delete;

    void begin_frame();
    // Submits the UI as an overlay to the window's default framebuffer.
    void render();

    bool wants_mouse() const;
    bool wants_keyboard() const;

private:
    void shutdown();

    base::Window& window_;
    ImGuiContext* context_ = nullptr;
    bool sdl_initialized_ = false;
    bool opengl_initialized_ = false;
};

} // namespace apps
