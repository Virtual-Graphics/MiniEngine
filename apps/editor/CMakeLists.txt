FetchContent_GetProperties(imgui SOURCE_DIR IMGUI_SOURCE_DIR)
include(${PROJECT_SOURCE_DIR}/cmake/third_party_editor.cmake)

add_library(vulcal_editor STATIC
    app/editor_context.cpp
    app/editor_layer.cpp
    dialogs/dialog.cpp
    dialogs/native_file_dialog.cpp
    dialogs/new_project_dialog.cpp
    panels/hierarchy_panel.cpp
    panels/inspector_panel.cpp
    panels/viewport_panel.cpp
    project/project_session.cpp
    scene/default_scene.cpp
    scene/editor_camera.cpp
    serialization/project_serializer.cpp
    serialization/scene_serializer.cpp
    "${IMGUI_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
)
target_link_libraries(vulcal_editor PUBLIC vulcal_app_imgui)
target_link_libraries(vulcal_editor PRIVATE yaml-cpp::yaml-cpp nfd::nfd)

add_executable(editor app/main.cpp)
target_link_libraries(editor PRIVATE vulcal_editor)
