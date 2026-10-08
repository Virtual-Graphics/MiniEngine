#include "base/application.h"
#include "base/layer.h"

#include <glad/glad.h>

#include <memory>

class ClearLayer : public base::Layer {
public:
    ClearLayer() : base::Layer("ClearLayer") {}

    void on_render() override {
        glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};

int main() {
    base::ApplicationCreateInfo info;
    info.width = 1280;
    info.height = 720;
    info.title = "MiniEngine";

    base::Application app(info);
    app.push_layer(std::make_unique<ClearLayer>());
    app.run();
    return 0;
}
