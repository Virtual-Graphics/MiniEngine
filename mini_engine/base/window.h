#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

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

    bool should_close() const { return m_should_close; }
    void poll_events();

    std::vector<const char*> required_instance_extensions() const;
    vk::SurfaceKHR create_surface(vk::Instance instance) const;

    uint32_t width() const { return m_width; }
    uint32_t height() const { return m_height; }
    bool resized() const { return m_resized; }
    void clear_resized() { m_resized = false; }

    SDL_Window* handle() const { return m_window; }

private:
    SDL_Window* m_window = nullptr;
    std::string m_title;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    bool m_should_close = false;
    bool m_resized = false;
};

} // namespace base
