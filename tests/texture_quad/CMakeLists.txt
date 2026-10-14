set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

add_executable(texture_quad main.cpp)

target_compile_definitions(texture_quad PRIVATE SHADERS_DIR="${SHADERS_DIR}")
target_compile_definitions(texture_quad PRIVATE ASSETS_DIR="${ASSETS_DIR}")

target_link_libraries(texture_quad PRIVATE mini_engine)
