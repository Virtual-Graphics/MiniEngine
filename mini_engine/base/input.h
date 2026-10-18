#pragma once

#include <utility>

#include "base/key_code.h"

struct SDL_Window;
union SDL_Event;

namespace base {

class Window;

class Input {
public:
    Input() = delete;

    // Queries are non-consuming and stable after Window::poll_events().
    // Pressed/released may both be true for a quick tap in one frame.
    static bool is_key_down(Key key);
    static bool is_key_pressed(Key key);
    static bool is_key_released(Key key);
    static bool is_mouse_button_down(MouseButton button);
    static bool is_mouse_button_pressed(MouseButton button);
    static bool is_mouse_button_released(MouseButton button);

    // Window logical coordinates; delta and wheel are accumulated for this frame.
    // Wheel Y is positive upwards, with SDL's flipped direction normalized.
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

    static void reset(SDL_Window* window = nullptr);
    static void begin_frame();
    static void process_event(const SDL_Event& event);
};

} // namespace base
