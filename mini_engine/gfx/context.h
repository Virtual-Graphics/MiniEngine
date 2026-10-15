#pragma once

struct SDL_Window;

namespace mini::gfx {

class Context {
public:
    // Sets the SDL GL attributes. Must be called before the window is created.
    static void prepare_attributes();

    explicit Context(SDL_Window* window);
    ~Context();

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    void swap_buffers() const;
    void set_swap_interval(int interval) const;

private:
    SDL_Window* window_ = nullptr;
    void* gl_context_ = nullptr;
};

} // namespace mini::gfx
