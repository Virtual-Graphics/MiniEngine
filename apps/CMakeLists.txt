set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

add_executable(mini_engine_app main.cpp)

target_compile_definitions(mini_engine_app PRIVATE SHADERS_DIR="${SHADERS_DIR}")

target_link_libraries(mini_engine_app PRIVATE mini_engine)


