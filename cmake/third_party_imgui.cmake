include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    # Docking release, commit b916186e66aa3c02b72c6ca17455516512ed8049.
    GIT_TAG        v1.93.0-docking
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
add_library(imgui::imgui ALIAS imgui)

target_include_directories(imgui SYSTEM PUBLIC "${imgui_SOURCE_DIR}")
target_link_libraries(imgui PRIVATE SDL3::SDL3 ${CMAKE_DL_LIBS})
