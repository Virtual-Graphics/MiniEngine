#include "engine/application.h"

#include <chrono>

#include "base/logger.h"
#include "engine/layer.h"
#include "gfx/command.h"

namespace engine {

Application::Application(const ApplicationCreateInfo& info) : info_(info) {
    base::initLogger();
    base::log::info("Starting application '{}' ({}x{})", info_.title, info_.width, info_.height);

    mini::gfx::Context::prepare_attributes();
    window_ = std::make_unique<base::Window>(base::WindowProps{ .title = info.title, .width = info.width, .height = info.height });
    context_ = std::make_unique<mini::gfx::Context>(window_->handle());
}

Application::~Application() {
    base::log::info("Shutting down application '{}'", info_.title);
    layer_stack_.detach_all();
    context_.reset();
    window_.reset();
    spdlog::default_logger()->flush();
}

void Application::push_layer(std::unique_ptr<Layer> layer) {
    base::log::debug("Pushing layer '{}'", layer->get_name());
    layer->application_ = this;
    layer_stack_.push_layer(std::move(layer));
}

void Application::push_overlay(std::unique_ptr<Layer> overlay) {
    base::log::debug("Pushing overlay '{}'", overlay->get_name());
    overlay->application_ = this;
    layer_stack_.push_overlay(std::move(overlay));
}

void Application::run() {
    base::log::info("Entering main loop");
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

        mini::gfx::Command::set_viewport(window_->width(), window_->height());

        for (auto& layer : layer_stack_) {
            layer->on_update(dt);
        }

        for (auto& layer : layer_stack_) {
            layer->on_render();
        }

        context_->swap_buffers();
    }

    running_ = false;
    base::log::info("Main loop exited");
}

} // namespace engine
