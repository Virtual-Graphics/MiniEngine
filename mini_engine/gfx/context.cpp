#include "gfx/context.h"

#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <stdexcept>
#include <string>

#include "base/logger.h"
#include "gfx/debug.h"

namespace mini::gfx {

void Context::prepare_attributes() {
    SDL_InitSubSystem(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

Context::Context(SDL_Window* window) : window_(window) {
    gl_context_ = SDL_GL_CreateContext(window_);
    if (!gl_context_) {
        throw std::runtime_error(std::string("SDL_GL_CreateContext failed: ") + SDL_GetError());
    }

    if (!SDL_GL_MakeCurrent(window_, static_cast<SDL_GLContext>(gl_context_))) {
        throw std::runtime_error(std::string("SDL_GL_MakeCurrent failed: ") + SDL_GetError());
    }
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        throw std::runtime_error("gladLoadGLLoader failed: could not load OpenGL functions");
    }

    base::log::info("OpenGL context created: {} ({} / {})",
                    reinterpret_cast<const char*>(glGetString(GL_VERSION)),
                    reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
                    reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    init_debug_output();
}

Context::~Context() {
    if (gl_context_) {
        SDL_GL_DestroyContext(static_cast<SDL_GLContext>(gl_context_));
    }
}

void Context::swap_buffers() const { SDL_GL_SwapWindow(window_); }

void Context::set_swap_interval(int interval) const { SDL_GL_SetSwapInterval(interval); }

} // namespace mini::gfx
