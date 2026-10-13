#pragma once

#include "base/timestep.h"

#include <string>

namespace engine {

class Application;

class Layer {
public:
    explicit Layer(const std::string& name = "Layer") : name_(name) {}

    virtual ~Layer() = default;

    virtual void on_attach() {}

    virtual void on_detach() {}

    virtual void on_update(base::Timestep dt) {}

    virtual void on_imgui_render() {}

    virtual void on_render() {}

    const std::string& get_name() const { return name_; }

protected:
    Application& application() { return *application_; }

private:
    friend class Application;

    std::string name_;
    Application* application_ = nullptr;
};

} // namespace engine
