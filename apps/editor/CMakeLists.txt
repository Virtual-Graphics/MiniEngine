FetchContent_GetProperties(imgui SOURCE_DIR IMGUI_SOURCE_DIR)

add_library(mini_editor STATIC
    editor_layer.cpp
    editor_context.cpp
    editor_camera.cpp
    default_scene.cpp
    panels/viewport_panel.cpp
    panels/hierarchy_panel.cpp
    panels/inspector_panel.cpp
    "${IMGUI_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
)
target_compile_definitions(mini_editor PRIVATE ASSETS_DIR="${PROJECT_SOURCE_DIR}/assets")
target_link_libraries(mini_editor PUBLIC mini_app_imgui)

add_executable(editor main.cpp)
target_link_libraries(editor PRIVATE mini_editor)
