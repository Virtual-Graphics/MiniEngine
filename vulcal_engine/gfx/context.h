#pragma once

#include <cstdint>

struct SDL_Window;

namespace vulcal::gfx {

class Context {
public:
    struct Limits {
        std::uint32_t max_texture_size = 0;
        std::uint32_t max_render_target_width = 0;
        std::uint32_t max_render_target_height = 0;
    };
    // Sets the SDL GL attributes. Must be called before the window is created.
    static void prepare_attributes();

    explicit Context(SDL_Window* window);
    ~Context();

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    void swap_buffers() const;
    void set_swap_interval(int interval) const;
    const Limits& limits() const { return limits_; }

private:
    SDL_Window* window_ = nullptr;
    void* gl_context_ = nullptr;
    Limits limits_;
};

} // namespace vulcal::gfx
