set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../shaders")
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../assets")

add_executable(render_to_texture main.cpp)

target_compile_definitions(render_to_texture PRIVATE SHADERS_DIR="${SHADERS_DIR}")
target_compile_definitions(render_to_texture PRIVATE ASSETS_DIR="${ASSETS_DIR}")

target_link_libraries(render_to_texture PRIVATE mini_engine)
