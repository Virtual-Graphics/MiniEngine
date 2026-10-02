#include "base/input.h"

#include <array>

#include <SDL3/SDL.h>

namespace base {
namespace {

constexpr std::array<SDL_Scancode, static_cast<size_t>(Key::Count)> k_scancodes{
    SDL_SCANCODE_UNKNOWN,
    SDL_SCANCODE_A, SDL_SCANCODE_B, SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E,
    SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_H, SDL_SCANCODE_I, SDL_SCANCODE_J,
    SDL_SCANCODE_K, SDL_SCANCODE_L, SDL_SCANCODE_M, SDL_SCANCODE_N, SDL_SCANCODE_O,
    SDL_SCANCODE_P, SDL_SCANCODE_Q, SDL_SCANCODE_R, SDL_SCANCODE_S, SDL_SCANCODE_T,
    SDL_SCANCODE_U, SDL_SCANCODE_V, SDL_SCANCODE_W, SDL_SCANCODE_X, SDL_SCANCODE_Y,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9,
    SDL_SCANCODE_ESCAPE, SDL_SCANCODE_RETURN, SDL_SCANCODE_TAB, SDL_SCANCODE_BACKSPACE,
    SDL_SCANCODE_SPACE, SDL_SCANCODE_DELETE, SDL_SCANCODE_INSERT,
    SDL_SCANCODE_HOME, SDL_SCANCODE_END, SDL_SCANCODE_PAGEUP, SDL_SCANCODE_PAGEDOWN,
    SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,
    SDL_SCANCODE_F1, SDL_SCANCODE_F2, SDL_SCANCODE_F3, SDL_SCANCODE_F4, SDL_SCANCODE_F5,
    SDL_SCANCODE_F6, SDL_SCANCODE_F7, SDL_SCANCODE_F8, SDL_SCANCODE_F9, SDL_SCANCODE_F10,
    SDL_SCANCODE_F11, SDL_SCANCODE_F12,
    SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT, SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL,
    SDL_SCANCODE_LALT, SDL_SCANCODE_RALT, SDL_SCANCODE_LGUI, SDL_SCANCODE_RGUI,
    SDL_SCANCODE_CAPSLOCK,
    SDL_SCANCODE_MINUS, SDL_SCANCODE_EQUALS, SDL_SCANCODE_LEFTBRACKET,
    SDL_SCANCODE_RIGHTBRACKET, SDL_SCANCODE_BACKSLASH, SDL_SCANCODE_SEMICOLON,
    SDL_SCANCODE_APOSTROPHE, SDL_SCANCODE_COMMA, SDL_SCANCODE_PERIOD, SDL_SCANCODE_SLASH,
    SDL_SCANCODE_GRAVE,
};

static_assert(static_cast<uint32_t>(MouseButton::Left) == SDL_BUTTON_LEFT);
static_assert(static_cast<uint32_t>(MouseButton::Middle) == SDL_BUTTON_MIDDLE);
static_assert(static_cast<uint32_t>(MouseButton::Right) == SDL_BUTTON_RIGHT);
static_assert(static_cast<uint32_t>(MouseButton::X1) == SDL_BUTTON_X1);
static_assert(static_cast<uint32_t>(MouseButton::X2) == SDL_BUTTON_X2);

float s_mouse_x = 0.0f;
float s_mouse_y = 0.0f;
float s_mouse_delta_x = 0.0f;
float s_mouse_delta_y = 0.0f;
float s_mouse_wheel_x = 0.0f;
float s_mouse_wheel_y = 0.0f;

} // namespace

bool Input::is_key_pressed(Key key) {
    if (key == Key::Unknown) {
        return false;
    }

    const bool* state = SDL_GetKeyboardState(nullptr);
    return state != nullptr && state[k_scancodes[static_cast<size_t>(key)]];
}

bool Input::is_mouse_button_pressed(MouseButton button) {
    const SDL_MouseButtonFlags flags = SDL_GetMouseState(nullptr, nullptr);
    return (flags & SDL_BUTTON_MASK(static_cast<uint32_t>(button))) != 0;
}

float Input::mouse_x() { return s_mouse_x; }

float Input::mouse_y() { return s_mouse_y; }

std::pair<float, float> Input::mouse_position() { return { s_mouse_x, s_mouse_y }; }

float Input::mouse_delta_x() { return s_mouse_delta_x; }

float Input::mouse_delta_y() { return s_mouse_delta_y; }

std::pair<float, float> Input::mouse_delta() { return { s_mouse_delta_x, s_mouse_delta_y }; }

float Input::mouse_wheel_x() { return s_mouse_wheel_x; }

float Input::mouse_wheel_y() { return s_mouse_wheel_y; }

std::pair<float, float> Input::mouse_wheel() { return { s_mouse_wheel_x, s_mouse_wheel_y }; }

void Input::refresh() {
    s_mouse_wheel_x = 0.0f;
    s_mouse_wheel_y = 0.0f;

    SDL_GetMouseState(&s_mouse_x, &s_mouse_y);
    SDL_GetRelativeMouseState(&s_mouse_delta_x, &s_mouse_delta_y);
}

void Input::on_mouse_wheel(float x, float y) {
    s_mouse_wheel_x += x;
    s_mouse_wheel_y += y;
}

} // namespace base
