file(GLOB_RECURSE MINI_ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(mini_engine STATIC ${MINI_ENGINE_SOURCES})

target_include_directories(mini_engine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_definitions(mini_engine PRIVATE
    MINI_RENDER_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/render/shaders"
)
target_link_libraries(mini_engine PUBLIC
    SDL3::SDL3
    glad::glad
    spdlog::spdlog
    glm::glm
    tinyobjloader::tinyobjloader
    stb::stb_image
    EnTT::EnTT
)
