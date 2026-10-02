#pragma once

#include "base/timestep.h"

#include <string>

namespace base {

class Application;

class Layer {
public:
    explicit Layer(const std::string& name = "Layer") : m_name(name) {}

    virtual ~Layer() = default;

    virtual void on_attach() {}

    virtual void on_detach() {}

    virtual void on_update(Timestep dt) {}

    virtual void on_imgui_render() {}

    virtual void on_render() {}

    const std::string& get_name() const { return m_name; }

protected:
    Application& application() { return *m_application; }

private:
    friend class Application;

    std::string m_name;
    Application* m_application = nullptr;
};

} // namespace base
