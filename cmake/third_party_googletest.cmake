include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.16.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK   OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(gtest PRIVATE -Wno-character-conversion)
    target_compile_options(gtest_main PRIVATE -Wno-character-conversion)
endif()
