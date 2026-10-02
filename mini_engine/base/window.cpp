#include "base/window.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <stdexcept>

#include "base/input.h"

namespace base {

Window::Window(const WindowProps& props)
        : m_title(props.title), m_width(props.width), m_height(props.height) {
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        throw std::runtime_error(std::string("SDL_InitSubSystem failed: ") + SDL_GetError());
    }

    m_window = SDL_CreateWindow(m_title.c_str(),
                                static_cast<int>(m_width),
                                static_cast<int>(m_height),
                                SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!m_window) {
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }

    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(m_window, &width, &height);
    m_width = static_cast<uint32_t>(width);
    m_height = static_cast<uint32_t>(height);
}

Window::~Window() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Window::poll_events() {
    Input::refresh();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            m_should_close = true;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            Input::on_mouse_wheel(event.wheel.x, event.wheel.y);
            break;
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
            int width = 0;
            int height = 0;
            SDL_GetWindowSizeInPixels(m_window, &width, &height);
            m_width = static_cast<uint32_t>(width);
            m_height = static_cast<uint32_t>(height);
            m_resized = true;
            break;
        }
        default:
            break;
        }
    }
}

std::vector<const char*> Window::required_instance_extensions() const {
    Uint32 count = 0;
    const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&count);
    if (!extensions) {
        throw std::runtime_error(std::string("SDL_Vulkan_GetInstanceExtensions failed: ") +
                                 SDL_GetError());
    }
    return std::vector<const char*>(extensions, extensions + count);
}

vk::SurfaceKHR Window::create_surface(vk::Instance instance) const {
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(m_window,
                                  static_cast<VkInstance>(instance),
                                  nullptr,
                                  &surface)) {
        throw std::runtime_error(std::string("SDL_Vulkan_CreateSurface failed: ") + SDL_GetError());
    }
    return vk::SurfaceKHR(surface);
}

} // namespace base
