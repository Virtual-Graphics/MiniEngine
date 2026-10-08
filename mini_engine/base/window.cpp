#include "base/window.h"

#include <glad/glad.h>

#include <SDL3/SDL.h>

#include <stdexcept>

#include "base/input.h"
#include "base/logger.h"

namespace base {

Window::Window(const WindowProps& props)
        : m_title(props.title), m_width(props.width), m_height(props.height) {
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        throw std::runtime_error(std::string("SDL_InitSubSystem failed: ") + SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    m_window = SDL_CreateWindow(m_title.c_str(),
                                static_cast<int>(m_width),
                                static_cast<int>(m_height),
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!m_window) {
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }

    m_gl_context = SDL_GL_CreateContext(m_window);
    if (!m_gl_context) {
        throw std::runtime_error(std::string("SDL_GL_CreateContext failed: ") +
                                 SDL_GetError());
    }

    if (!SDL_GL_MakeCurrent(m_window, m_gl_context)) {
        throw std::runtime_error(std::string("SDL_GL_MakeCurrent failed: ") + SDL_GetError());
    }
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        throw std::runtime_error("gladLoadGLLoader failed: could not load OpenGL functions");
    }

    log::info("OpenGL context created: {} ({} / {})",
              reinterpret_cast<const char*>(glGetString(GL_VERSION)),
              reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
              reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(m_window, &width, &height);
    m_width = static_cast<uint32_t>(width);
    m_height = static_cast<uint32_t>(height);
}

Window::~Window() {
    if (m_gl_context) {
        SDL_GL_DestroyContext(m_gl_context);
    }
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

void Window::swap_buffers() { SDL_GL_SwapWindow(m_window); }

} // namespace base
