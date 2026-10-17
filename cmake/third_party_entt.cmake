include(FetchContent)

FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        v4.0.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
    FIND_PACKAGE_ARGS NAMES EnTT
)

set(ENTT_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENTT_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
set(ENTT_BUILD_DOCS    OFF CACHE BOOL "" FORCE)
set(ENTT_INSTALL       OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(entt)