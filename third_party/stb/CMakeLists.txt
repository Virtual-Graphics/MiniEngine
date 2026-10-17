add_library(stb STATIC
    src/stb_image.cpp
)

add_library(stb::stb_image ALIAS stb)

target_include_directories(stb
    SYSTEM PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
