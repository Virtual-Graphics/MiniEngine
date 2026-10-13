#include "engine/application.h"

#include <chrono>

#include "engine/layer.h"

namespace engine {

Application::Application(const ApplicationCreateInfo& info) : info_(info) {
    window_ = std::make_unique<base::Window>(base::WindowProps{ .title = info.title, .width = info.width, .height = info.height });
    context_ = std::make_unique<mini::gfx::Context>(window_->handle());
}

Application::~Application() {
    layer_stack_.detach_all();
    context_.reset();
    window_.reset();
}

void Application::push_layer(std::unique_ptr<Layer> layer) {
    layer->application_ = this;
    layer_stack_.push_layer(std::move(layer));
}

void Application::push_overlay(std::unique_ptr<Layer> overlay) {
    overlay->application_ = this;
    layer_stack_.push_overlay(std::move(overlay));
}

void Application::run() {
    running_ = true;
    auto last = std::chrono::high_resolution_clock::now();

    while (running_ && !window_->should_close()) {
        window_->poll_events();

        const auto now = std::chrono::high_resolution_clock::now();
        const base::Timestep dt(std::chrono::duration<float>(now - last).count());
        last = now;

        if (window_->width() == 0 || window_->height() == 0) {
            continue;
        }

        context_->set_viewport(window_->width(), window_->height());

        for (auto& layer : layer_stack_) {
            layer->on_update(dt);
        }

        for (auto& layer : layer_stack_) {
            layer->on_render();
        }

        context_->swap_buffers();
    }

    running_ = false;
}

} // namespace engine
