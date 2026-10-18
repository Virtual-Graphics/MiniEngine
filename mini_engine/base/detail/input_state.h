#pragma once

#include "base/key_code.h"
#include <array>
#include <cstddef>
#include <utility>

struct SDL_Window;
union SDL_Event;

namespace base::detail {

// Single-window event accumulator; queries are stable after Window::poll_events().
class InputState {
public:
    void reset(SDL_Window* window);
    void begin_frame();
    void process_event(const SDL_Event& event);
    bool is_key_down(Key key) const;
    bool is_key_pressed(Key key) const;
    bool is_key_released(Key key) const;
    bool is_mouse_button_down(MouseButton button) const;
    bool is_mouse_button_pressed(MouseButton button) const;
    bool is_mouse_button_released(MouseButton button) const;
    std::pair<float, float> mouse_position() const { return { mouse_x_, mouse_y_ }; }
    std::pair<float, float> mouse_delta() const { return { delta_x_, delta_y_ }; }
    std::pair<float, float> mouse_wheel() const { return { wheel_x_, wheel_y_ }; }

private:
    struct Button {
        bool down = false;
        bool pressed = false;
        bool released = false;
        void update(bool value, bool repeat = false);
    };
    static bool valid(Key key);
    static bool valid(MouseButton button);
    void synchronize();
    void lose_focus();
    std::array<Button, static_cast<std::size_t>(Key::Count)> keys_{};
    std::array<Button, 6> buttons_{};
    bool focused_ = false;
    float mouse_x_ = 0.0f;
    float mouse_y_ = 0.0f;
    float delta_x_ = 0.0f;
    float delta_y_ = 0.0f;
    float wheel_x_ = 0.0f;
    float wheel_y_ = 0.0f;
};

} // namespace base::detail
