include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-3.2.22
    GIT_PROGRESS   TRUE
    GIT_SHALLOW    TRUE
    FIND_PACKAGE_ARGS 
)

set(SDL_SHARED OFF CACHE BOOL "Build a shared version of SDL3")
set(SDL_STATIC ON CACHE BOOL "Build a static version of SDL3")

FetchContent_MakeAvailable(SDL3)
