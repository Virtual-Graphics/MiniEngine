set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

add_executable(model_viewer main.cpp)

target_compile_definitions(model_viewer PRIVATE SHADERS_DIR="${SHADERS_DIR}")
target_compile_definitions(model_viewer PRIVATE ASSETS_DIR="${ASSETS_DIR}")

target_link_libraries(model_viewer PRIVATE mini_engine)
