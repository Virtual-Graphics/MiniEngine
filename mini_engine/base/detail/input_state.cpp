#include "base/detail/input_state.h"
#include <SDL3/SDL.h>
#include <algorithm>

namespace base::detail {
namespace {
constexpr std::array<SDL_Scancode, static_cast<size_t>(Key::Count)> k_scancodes{
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_A,
    SDL_SCANCODE_B,
    SDL_SCANCODE_C,
    SDL_SCANCODE_D,
    SDL_SCANCODE_E,
    SDL_SCANCODE_F,
    SDL_SCANCODE_G,
    SDL_SCANCODE_H,
    SDL_SCANCODE_I,
    SDL_SCANCODE_J,
    SDL_SCANCODE_K,
    SDL_SCANCODE_L,
    SDL_SCANCODE_M,
    SDL_SCANCODE_N,
    SDL_SCANCODE_O,
    SDL_SCANCODE_P,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_R,
    SDL_SCANCODE_S,
    SDL_SCANCODE_T,
    SDL_SCANCODE_U,
    SDL_SCANCODE_V,
    SDL_SCANCODE_W,
    SDL_SCANCODE_X,
    SDL_SCANCODE_Y,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_0,
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_4,
    SDL_SCANCODE_5,
    SDL_SCANCODE_6,
    SDL_SCANCODE_7,
    SDL_SCANCODE_8,
    SDL_SCANCODE_9,
    SDL_SCANCODE_ESCAPE,
    SDL_SCANCODE_RETURN,
    SDL_SCANCODE_TAB,
    SDL_SCANCODE_BACKSPACE,
    SDL_SCANCODE_SPACE,
    SDL_SCANCODE_DELETE,
    SDL_SCANCODE_INSERT,
    SDL_SCANCODE_HOME,
    SDL_SCANCODE_END,
    SDL_SCANCODE_PAGEUP,
    SDL_SCANCODE_PAGEDOWN,
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_F1,
    SDL_SCANCODE_F2,
    SDL_SCANCODE_F3,
    SDL_SCANCODE_F4,
    SDL_SCANCODE_F5,
    SDL_SCANCODE_F6,
    SDL_SCANCODE_F7,
    SDL_SCANCODE_F8,
    SDL_SCANCODE_F9,
    SDL_SCANCODE_F10,
    SDL_SCANCODE_F11,
    SDL_SCANCODE_F12,
    SDL_SCANCODE_LSHIFT,
    SDL_SCANCODE_RSHIFT,
    SDL_SCANCODE_LCTRL,
    SDL_SCANCODE_RCTRL,
    SDL_SCANCODE_LALT,
    SDL_SCANCODE_RALT,
    SDL_SCANCODE_LGUI,
    SDL_SCANCODE_RGUI,
    SDL_SCANCODE_CAPSLOCK,
    SDL_SCANCODE_MINUS,
    SDL_SCANCODE_EQUALS,
    SDL_SCANCODE_LEFTBRACKET,
    SDL_SCANCODE_RIGHTBRACKET,
    SDL_SCANCODE_BACKSLASH,
    SDL_SCANCODE_SEMICOLON,
    SDL_SCANCODE_APOSTROPHE,
    SDL_SCANCODE_COMMA,
    SDL_SCANCODE_PERIOD,
    SDL_SCANCODE_SLASH,
    SDL_SCANCODE_GRAVE,
};

static_assert(static_cast<uint32_t>(MouseButton::Left) == SDL_BUTTON_LEFT);
static_assert(static_cast<uint32_t>(MouseButton::Middle) == SDL_BUTTON_MIDDLE);
static_assert(static_cast<uint32_t>(MouseButton::Right) == SDL_BUTTON_RIGHT);
static_assert(static_cast<uint32_t>(MouseButton::X1) == SDL_BUTTON_X1);
static_assert(static_cast<uint32_t>(MouseButton::X2) == SDL_BUTTON_X2);

} // namespace

void InputState::Button::update(bool value, bool repeat) {
    // SDL's focus snapshot may already reflect the final state of queued events.
    // Preserve the transitions carried by those events independently of that snapshot.
    if (value && !repeat) pressed = true;
    if (!value) released = true;
    down = value;
}

bool InputState::valid(Key key) { return key > Key::Unknown && key < Key::Count; }
bool InputState::valid(MouseButton button) { return button >= MouseButton::Left && button <= MouseButton::X2; }
bool InputState::is_key_down(Key key) const { return valid(key) && keys_[static_cast<std::size_t>(key)].down; }
bool InputState::is_key_pressed(Key key) const { return valid(key) && keys_[static_cast<std::size_t>(key)].pressed; }
bool InputState::is_key_released(Key key) const { return valid(key) && keys_[static_cast<std::size_t>(key)].released; }
bool InputState::is_mouse_button_down(MouseButton button) const { return valid(button) && buttons_[static_cast<std::size_t>(button)].down; }
bool InputState::is_mouse_button_pressed(MouseButton button) const {
    return valid(button) && buttons_[static_cast<std::size_t>(button)].pressed;
}
bool InputState::is_mouse_button_released(MouseButton button) const {
    return valid(button) && buttons_[static_cast<std::size_t>(button)].released;
}

void InputState::reset(SDL_Window* window) {
    *this = {};
    if (window && (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0) {
        focused_ = true;
        synchronize();
    }
}
void InputState::begin_frame() {
    for (auto& key: keys_) key.pressed = key.released = false;
    for (auto& button: buttons_) button.pressed = button.released = false;
    delta_x_ = delta_y_ = wheel_x_ = wheel_y_ = 0.0f;
}
void InputState::synchronize() {
    int count = 0;
    const auto* keyboard = SDL_GetKeyboardState(&count);
    for (std::size_t i = 1; i < keys_.size(); ++i) keys_[i].down = keyboard && k_scancodes[i] < count && keyboard[k_scancodes[i]];
    const auto mouse = SDL_GetMouseState(&mouse_x_, &mouse_y_);
    for (std::size_t i = 1; i < buttons_.size(); ++i) buttons_[i].down = (mouse & SDL_BUTTON_MASK(i)) != 0;
    delta_x_ = delta_y_ = wheel_x_ = wheel_y_ = 0.0f;
}
void InputState::lose_focus() {
    focused_ = false;
    for (auto& key: keys_)
        if (key.down) key.update(false);
    for (auto& button: buttons_)
        if (button.down) button.update(false);
    delta_x_ = delta_y_ = wheel_x_ = wheel_y_ = 0.0f;
}
void InputState::process_event(const SDL_Event& event) {
    if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
        lose_focus();
        return;
    }
    if (event.type == SDL_EVENT_WINDOW_FOCUS_GAINED) {
        focused_ = true;
        synchronize();
        return;
    }
    if (!focused_) return;
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            const auto found = std::find(k_scancodes.begin() + 1, k_scancodes.end(), event.key.scancode);
            if (found != k_scancodes.end())
                keys_[static_cast<std::size_t>(found - k_scancodes.begin())].update(event.type == SDL_EVENT_KEY_DOWN, event.key.repeat);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (valid(static_cast<MouseButton>(event.button.button)))
                buttons_[event.button.button].update(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            mouse_x_ = event.button.x;
            mouse_y_ = event.button.y;
            break;
        case SDL_EVENT_MOUSE_MOTION:
            mouse_x_ = event.motion.x;
            mouse_y_ = event.motion.y;
            delta_x_ += event.motion.xrel;
            delta_y_ += event.motion.yrel;
            break;
        case SDL_EVENT_MOUSE_WHEEL: {
            const float sign = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1.0f : 1.0f;
            wheel_x_ += sign * event.wheel.x;
            wheel_y_ += sign * event.wheel.y;
            break;
        }
        default:
            break;
    }
}
} // namespace base::detail
