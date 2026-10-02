include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.17.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
    FIND_PACKAGE_ARGS   
)

set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)   
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE) 
set(SPDLOG_BUILD_TESTS  OFF CACHE BOOL "" FORCE) 

FetchContent_MakeAvailable(spdlog)