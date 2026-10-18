#include "base/window.h"

#include <SDL3/SDL.h>

#include <stdexcept>
#include <utility>

#include "base/input.h"

namespace base {

Window::Window(const WindowProps& props) : title_(props.title), width_(props.width), height_(props.height) {
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        throw std::runtime_error(std::string("SDL_InitSubSystem failed: ") + SDL_GetError());
    }

    window_ = SDL_CreateWindow(title_.c_str(),
                                static_cast<int>(width_),
                                static_cast<int>(height_),
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window_) {
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }

    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(window_, &width, &height);
    width_ = static_cast<uint32_t>(width);
    height_ = static_cast<uint32_t>(height);
}

Window::~Window() {
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Window::poll_events() {
    Input::refresh();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event_callback_) {
            event_callback_(event);
        }

        switch (event.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            should_close_ = true;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            Input::on_mouse_wheel(event.wheel.x, event.wheel.y);
            break;
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
            int width = 0;
            int height = 0;
            SDL_GetWindowSizeInPixels(window_, &width, &height);
            width_ = static_cast<uint32_t>(width);
            height_ = static_cast<uint32_t>(height);
            resized_ = true;
            break;
        }
        default:
            break;
        }
    }
}

void Window::set_event_callback(EventCallback callback) { event_callback_ = std::move(callback); }

} // namespace base
