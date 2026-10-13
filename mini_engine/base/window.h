#pragma once

#include <cstdint>
#include <string>

struct SDL_Window;

namespace base {

struct WindowProps {
    std::string title = "MiniEngine";
    uint32_t width = 1280;
    uint32_t height = 720;
};

class Window {
public:
    explicit Window(const WindowProps& props);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool should_close() const { return should_close_; }
    void poll_events();

    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }
    bool resized() const { return resized_; }
    void clear_resized() { resized_ = false; }

    SDL_Window* handle() const { return window_; }

private:
    SDL_Window* window_ = nullptr;
    std::string title_;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    bool should_close_ = false;
    bool resized_ = false;
};

} // namespace base
