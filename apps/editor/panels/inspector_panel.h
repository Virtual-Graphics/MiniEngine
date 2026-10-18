#pragma once

namespace editor {

class EditorContext;

class InspectorPanel {
public:
    void draw(EditorContext& context, bool& open) const;
};

} // namespace editor
