#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "base/timestep.h"
#include "base/window.h"
#include "engine/layer_stack.h"
#include "gfx/context.h"

namespace engine {

struct ApplicationCreateInfo {
    uint32_t width = 1280;
    uint32_t height = 720;
    std::string title = "MiniEngine";
};

class Application {
public:
    explicit Application(const ApplicationCreateInfo& info);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

    base::Window& get_window() { return *window_; }
    const mini::gfx::Context& get_graphics_context() const { return *context_; }
    LayerStack& get_layer_stack() { return layer_stack_; }

    void push_layer(std::unique_ptr<Layer> layer);
    void push_overlay(std::unique_ptr<Layer> overlay);

private:
    ApplicationCreateInfo info_;
    std::unique_ptr<base::Window> window_;
    std::unique_ptr<mini::gfx::Context> context_;
    LayerStack layer_stack_;
    bool running_ = false;
};

} // namespace engine
