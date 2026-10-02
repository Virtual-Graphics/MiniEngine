#include "base/application.h"

#include <chrono>

#include <vulcao/context.h>
#include <vulcao/frame_manager.h>

#include "base/layer.h"
#include "base/logger.h"

namespace base {

Application::Application(const ApplicationCreateInfo& info) : m_info(info) {
    m_window = std::make_unique<Window>(
        WindowProps{.title = info.title, .width = info.width, .height = info.height});

    vulcao::ContextInfo context_info;
    context_info.app_name = info.title;
    context_info.extensions = m_window->required_instance_extensions();
    m_context = std::make_unique<vulcao::Context>(context_info);

    const vk::SurfaceKHR surface = m_window->create_surface(m_context->instance());

    vk::Extent2D extent;
    extent.width = m_window->width();
    extent.height = m_window->height();
    m_context->initialize(surface, extent);

    m_frame_manager = std::make_unique<vulcao::FrameManager>(*m_context);
    log::info("MiniEngine running on {}", m_context->physical_device_info().name);
}

Application::~Application() {
    if (m_context) {
        m_context->wait_idle();
    }
    m_layer_stack.detach_all();
    m_frame_manager.reset();
    m_context.reset();
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

        for (auto& layer : m_layer_stack) {
            layer->on_update(dt);
        }

        for (auto& layer : m_layer_stack) {
            layer->on_render();
        }
    }

    m_running = false;
}

} // namespace base
