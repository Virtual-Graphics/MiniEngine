#include "editor/editor_layer.h"
#include "engine/application.h"

#include <memory>

int main() {
    engine::Application app({ .width = 1600, .height = 900, .title = "MiniEngine - Editor" });
    app.push_layer(std::make_unique<editor::EditorLayer>());
    app.run();
    return 0;
}
