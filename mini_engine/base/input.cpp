#include "base/input.h"
#include "base/detail/input_state.h"

namespace base {
namespace {
detail::InputState state;
}
bool Input::is_key_down(Key key) { return state.is_key_down(key); }
bool Input::is_key_pressed(Key key) { return state.is_key_pressed(key); }
bool Input::is_key_released(Key key) { return state.is_key_released(key); }
bool Input::is_mouse_button_down(MouseButton button) { return state.is_mouse_button_down(button); }
bool Input::is_mouse_button_pressed(MouseButton button) { return state.is_mouse_button_pressed(button); }
bool Input::is_mouse_button_released(MouseButton button) { return state.is_mouse_button_released(button); }
float Input::mouse_x() { return state.mouse_position().first; }
float Input::mouse_y() { return state.mouse_position().second; }
std::pair<float, float> Input::mouse_position() { return state.mouse_position(); }
float Input::mouse_delta_x() { return state.mouse_delta().first; }
float Input::mouse_delta_y() { return state.mouse_delta().second; }
std::pair<float, float> Input::mouse_delta() { return state.mouse_delta(); }
float Input::mouse_wheel_x() { return state.mouse_wheel().first; }
float Input::mouse_wheel_y() { return state.mouse_wheel().second; }
std::pair<float, float> Input::mouse_wheel() { return state.mouse_wheel(); }
void Input::reset(SDL_Window* window) { state.reset(window); }
void Input::begin_frame() { state.begin_frame(); }
void Input::process_event(const SDL_Event& event) { state.process_event(event); }
} // namespace base
