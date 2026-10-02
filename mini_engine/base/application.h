#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "base/layer_stack.h"
#include "base/timestep.h"
#include "base/window.h"

namespace vulcao {
class Context;
class FrameManager;
} // namespace vulcao

namespace base {

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

    Window& get_window() { return *m_window; }
    vulcao::Context& get_context() { return *m_context; }
    vulcao::FrameManager& get_frame_manager() { return *m_frame_manager; }
    LayerStack& get_layer_stack() { return m_layer_stack; }

    void push_layer(std::unique_ptr<Layer> layer);
    void push_overlay(std::unique_ptr<Layer> overlay);

private:
    ApplicationCreateInfo m_info;
    std::unique_ptr<Window> m_window;
    std::unique_ptr<vulcao::Context> m_context;
    std::unique_ptr<vulcao::FrameManager> m_frame_manager;
    LayerStack m_layer_stack;
    bool m_running = false;
};

} // namespace base
