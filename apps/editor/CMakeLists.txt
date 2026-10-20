FetchContent_GetProperties(imgui SOURCE_DIR IMGUI_SOURCE_DIR)
include(${PROJECT_SOURCE_DIR}/cmake/third_party_editor.cmake)

add_library(vulcal_editor STATIC
    editor_layer.cpp
    editor_context.cpp
    editor_camera.cpp
    default_scene.cpp
    project_serializer.cpp
    project_session.cpp
    scene_serializer.cpp
    dialog.cpp
    native_file_dialog.cpp
    new_project_dialog.cpp
    panels/viewport_panel.cpp
    panels/hierarchy_panel.cpp
    panels/inspector_panel.cpp
    "${IMGUI_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
)
target_link_libraries(vulcal_editor PUBLIC vulcal_app_imgui)
target_link_libraries(vulcal_editor PRIVATE yaml-cpp::yaml-cpp nfd::nfd)

add_executable(editor main.cpp)
target_link_libraries(editor PRIVATE vulcal_editor)
