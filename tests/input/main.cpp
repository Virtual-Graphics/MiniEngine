#include "base/detail/input_state.h"

#include <SDL3/SDL.h>
#include <gtest/gtest.h>

namespace {

SDL_Event key_event(Uint32 type, SDL_Scancode code, bool repeat = false) {
    SDL_Event event{};
    event.type = type;
    event.key.scancode = code;
    event.key.repeat = repeat;
    return event;
}

SDL_Event button_event(Uint32 type, Uint8 button) {
    SDL_Event event{};
    event.type = type;
    event.button.button = button;
    return event;
}

class InputFrame : public testing::Test {
protected:
    void SetUp() override {
        SDL_Event focus{};
        focus.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
        input.process_event(focus);
        input.begin_frame();
    }
    base::detail::InputState input;
};

TEST_F(InputFrame, HoldDoesNotRepeatPressedAndReleaseLastsOneFrame) {
    input.process_event(key_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_W));
    EXPECT_TRUE(input.is_key_down(base::Key::W));
    EXPECT_TRUE(input.is_key_pressed(base::Key::W));
    EXPECT_TRUE(input.is_key_pressed(base::Key::W));
    input.begin_frame();
    input.process_event(key_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_W, true));
    EXPECT_TRUE(input.is_key_down(base::Key::W));
    EXPECT_FALSE(input.is_key_pressed(base::Key::W));
    input.process_event(key_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_W));
    EXPECT_FALSE(input.is_key_down(base::Key::W));
    EXPECT_TRUE(input.is_key_released(base::Key::W));
    input.begin_frame();
    EXPECT_FALSE(input.is_key_released(base::Key::W));
}

TEST_F(InputFrame, QuickTapRetainsBothTransitions) {
    input.process_event(key_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_A));
    input.process_event(key_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_A));
    EXPECT_TRUE(input.is_key_pressed(base::Key::A));
    EXPECT_TRUE(input.is_key_released(base::Key::A));
    EXPECT_FALSE(input.is_key_down(base::Key::A));
}

TEST_F(InputFrame, NonRepeatPressIsObservableWhenHeldStateWasAlreadySynchronized) {
    input.process_event(key_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_W));
    input.begin_frame();
    // A focus snapshot can already contain the final held state when this event arrives.
    input.process_event(key_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_W));
    EXPECT_TRUE(input.is_key_pressed(base::Key::W));
}

TEST_F(InputFrame, ReleaseIsObservableWhenFocusSnapshotAlreadyReportsKeyUp) {
    input.process_event(key_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_W));
    input.process_event(button_event(SDL_EVENT_MOUSE_BUTTON_UP, SDL_BUTTON_LEFT));
    EXPECT_TRUE(input.is_key_released(base::Key::W));
    EXPECT_TRUE(input.is_mouse_button_released(base::MouseButton::Left));
}

TEST_F(InputFrame, MouseButtonsHaveIndependentFrameTransitions) {
    input.process_event(button_event(SDL_EVENT_MOUSE_BUTTON_DOWN, SDL_BUTTON_RIGHT));
    input.process_event(button_event(SDL_EVENT_MOUSE_BUTTON_DOWN, SDL_BUTTON_LEFT));
    input.process_event(button_event(SDL_EVENT_MOUSE_BUTTON_UP, SDL_BUTTON_LEFT));
    EXPECT_TRUE(input.is_mouse_button_pressed(base::MouseButton::Left));
    EXPECT_TRUE(input.is_mouse_button_released(base::MouseButton::Left));
    EXPECT_FALSE(input.is_mouse_button_down(base::MouseButton::Left));
    EXPECT_TRUE(input.is_mouse_button_down(base::MouseButton::Right));
    input.begin_frame();
    EXPECT_FALSE(input.is_mouse_button_pressed(base::MouseButton::Right));
    EXPECT_TRUE(input.is_mouse_button_down(base::MouseButton::Right));
}

TEST_F(InputFrame, MotionAndWheelAccumulateWithoutBeingConsumed) {
    SDL_Event motion{};
    motion.type = SDL_EVENT_MOUSE_MOTION;
    motion.motion.x = 50.0f;
    motion.motion.y = 60.0f;
    motion.motion.xrel = 3.0f;
    motion.motion.yrel = -2.0f;
    input.process_event(motion);
    motion.motion.x = 54.0f;
    motion.motion.xrel = 4.0f;
    input.process_event(motion);
    SDL_Event wheel{};
    wheel.type = SDL_EVENT_MOUSE_WHEEL;
    wheel.wheel.x = 0.25f;
    wheel.wheel.y = 1.5f;
    input.process_event(wheel);
    wheel.wheel.direction = SDL_MOUSEWHEEL_FLIPPED;
    wheel.wheel.x = 0.5f;
    wheel.wheel.y = 0.25f;
    input.process_event(wheel);
    EXPECT_EQ(input.mouse_position(), (std::pair{ 54.0f, 60.0f }));
    EXPECT_EQ(input.mouse_delta(), (std::pair{ 7.0f, -4.0f }));
    EXPECT_EQ(input.mouse_delta(), (std::pair{ 7.0f, -4.0f }));
    EXPECT_EQ(input.mouse_wheel(), (std::pair{ -0.25f, 1.25f }));
    input.begin_frame();
    EXPECT_EQ(input.mouse_position(), (std::pair{ 54.0f, 60.0f }));
    EXPECT_EQ(input.mouse_delta(), (std::pair{ 0.0f, 0.0f }));
    EXPECT_EQ(input.mouse_wheel(), (std::pair{ 0.0f, 0.0f }));
}

TEST_F(InputFrame, FocusLossReleasesHeldInputsAndIgnoresUnfocusedEvents) {
    input.process_event(key_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_W));
    input.process_event(button_event(SDL_EVENT_MOUSE_BUTTON_DOWN, SDL_BUTTON_RIGHT));
    SDL_Event event{};
    event.type = SDL_EVENT_MOUSE_MOTION;
    event.motion.xrel = 8.0f;
    input.process_event(event);
    event.type = SDL_EVENT_WINDOW_FOCUS_LOST;
    input.process_event(event);
    EXPECT_FALSE(input.is_key_down(base::Key::W));
    EXPECT_TRUE(input.is_key_released(base::Key::W));
    EXPECT_FALSE(input.is_mouse_button_down(base::MouseButton::Right));
    EXPECT_TRUE(input.is_mouse_button_released(base::MouseButton::Right));
    EXPECT_FALSE(input.is_key_released(base::Key::A));
    EXPECT_FALSE(input.is_mouse_button_released(base::MouseButton::Left));
    EXPECT_EQ(input.mouse_delta(), (std::pair{ 0.0f, 0.0f }));
    input.process_event(key_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_W));
    EXPECT_FALSE(input.is_key_down(base::Key::W));
    input.begin_frame();
    event.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
    input.process_event(event);
    EXPECT_FALSE(input.is_key_pressed(base::Key::W));
    EXPECT_FALSE(input.is_mouse_button_pressed(base::MouseButton::Right));
}

TEST_F(InputFrame, ResetClearsPreviousWindowState) {
    input.process_event(key_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_W));
    input.reset(nullptr);
    EXPECT_FALSE(input.is_key_down(base::Key::W));
    EXPECT_FALSE(input.is_key_pressed(base::Key::W));
    EXPECT_FALSE(input.is_key_released(base::Key::W));
}

TEST_F(InputFrame, InvalidEnumsAndUnmappedEventsAreHarmless) {
    input.process_event(key_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_UNKNOWN));
    input.process_event(button_event(SDL_EVENT_MOUSE_BUTTON_DOWN, 255));
    for (const auto key: { base::Key::Unknown, base::Key::Count, static_cast<base::Key>(65535) }) {
        EXPECT_FALSE(input.is_key_down(key));
        EXPECT_FALSE(input.is_key_pressed(key));
        EXPECT_FALSE(input.is_key_released(key));
    }
    for (const auto button: { static_cast<base::MouseButton>(0), static_cast<base::MouseButton>(255) }) {
        EXPECT_FALSE(input.is_mouse_button_down(button));
        EXPECT_FALSE(input.is_mouse_button_pressed(button));
        EXPECT_FALSE(input.is_mouse_button_released(button));
    }
}

} // namespace
