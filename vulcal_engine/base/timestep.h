#pragma once

namespace base {

class Timestep {
public:
    Timestep(float time = 0.0f) : time_(time) {}

    operator float() const { return time_; }

    float get_seconds() const { return time_; }

    float get_milliseconds() const { return time_ * 1000.0f; }

private:
    float time_{ 0.0f };
};

} // namespace base
