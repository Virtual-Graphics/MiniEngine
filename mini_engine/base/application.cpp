#include "base/application.h"

#include <chrono>

#include <glad/glad.h>

#include "base/layer.h"

namespace base {

Application::Application(const ApplicationCreateInfo& info) : m_info(info) {
    m_window = std::make_unique<Window>(
        WindowProps{.title = info.title, .width = info.width, .height = info.height});
}

Application::~Application() {
    m_layer_stack.detach_all();
    m_window.reset();
}

void Application::push_layer(std::unique_ptr<Layer> layer) {
    layer->m_application = this;
    m_layer_stack.push_layer(std::move(layer));
}

void Application::push_overlay(std::unique_ptr<Layer> overlay) {
    overlay->m_application = this;
    m_layer_stack.push_overlay(std::move(overlay));
}

void Application::run() {
    m_running = true;
    auto last = std::chrono::high_resolution_clock::now();

    while (m_running && !m_window->should_close()) {
        m_window->poll_events();

        const auto now = std::chrono::high_resolution_clock::now();
        const Timestep dt(std::chrono::duration<float>(now - last).count());
        last = now;

        if (m_window->width() == 0 || m_window->height() == 0) {
            continue;
        }

        glViewport(0,
                   0,
                   static_cast<GLsizei>(m_window->width()),
                   static_cast<GLsizei>(m_window->height()));

        for (auto& layer : m_layer_stack) {
            layer->on_update(dt);
        }

        for (auto& layer : m_layer_stack) {
            layer->on_render();
        }

        m_window->swap_buffers();
    }

    m_running = false;
}

} // namespace base
