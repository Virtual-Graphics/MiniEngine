add_library(tinyobjloader STATIC
    src/tiny_obj_loader.cc
)

add_library(tinyobjloader::tinyobjloader ALIAS tinyobjloader)

target_include_directories(tinyobjloader
    SYSTEM PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
