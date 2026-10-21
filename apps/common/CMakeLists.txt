include("${PROJECT_SOURCE_DIR}/cmake/third_party_imgui.cmake")

add_library(vulcal_app_imgui STATIC imgui_integration.cpp imgui_render_target.cpp)

target_include_directories(vulcal_app_imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(vulcal_app_imgui PUBLIC vulcal_engine imgui::imgui)

add_subdirectory(asset)
