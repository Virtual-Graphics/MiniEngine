#pragma once

#include <utility>

#include "base/key_code.h"

namespace base {

class Window;

class Input {
public:
    Input() = delete;

    static bool is_key_pressed(Key key);
    static bool is_mouse_button_pressed(MouseButton button);

    static float mouse_x();
    static float mouse_y();
    static std::pair<float, float> mouse_position();

    static float mouse_delta_x();
    static float mouse_delta_y();
    static std::pair<float, float> mouse_delta();

    static float mouse_wheel_x();
    static float mouse_wheel_y();
    static std::pair<float, float> mouse_wheel();

private:
    friend class Window;

    static void refresh();
    static void on_mouse_wheel(float x, float y);
};

} // namespace base
