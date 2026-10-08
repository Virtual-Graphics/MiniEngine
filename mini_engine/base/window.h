#pragma once

#include <cstdint>
#include <string>

struct SDL_Window;
struct SDL_GLContextState;

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

    bool should_close() const { return m_should_close; }
    void poll_events();
    void swap_buffers();

    uint32_t width() const { return m_width; }
    uint32_t height() const { return m_height; }
    bool resized() const { return m_resized; }
    void clear_resized() { m_resized = false; }

    SDL_Window* handle() const { return m_window; }

private:
    SDL_Window* m_window = nullptr;
    SDL_GLContextState* m_gl_context = nullptr;
    std::string m_title;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    bool m_should_close = false;
    bool m_resized = false;
};

} // namespace base
