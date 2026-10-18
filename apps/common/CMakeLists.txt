include("${PROJECT_SOURCE_DIR}/cmake/third_party_imgui.cmake")

add_library(mini_app_imgui STATIC imgui_integration.cpp imgui_render_target.cpp)

target_include_directories(mini_app_imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(mini_app_imgui PUBLIC mini_engine imgui::imgui)
