add_executable(editor_state_test
    main.cpp
    ../../apps/editor/app/editor_context.cpp
    ../../apps/editor/scene/editor_camera.cpp
)
target_include_directories(editor_state_test PRIVATE ../../apps)
target_link_libraries(editor_state_test PRIVATE vulcal_engine GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(editor_state_test)

if (BUILD_APPS)
    add_executable(editor_persistence_test persistence.cpp)
    target_link_libraries(editor_persistence_test PRIVATE vulcal_editor GTest::gtest_main)
    gtest_discover_tests(editor_persistence_test)

    # Headless ImGui interaction tests; no window or OpenGL context is required.
    add_executable(editor_panel_test panels.cpp)
    target_link_libraries(editor_panel_test PRIVATE vulcal_editor GTest::gtest_main)
    gtest_discover_tests(editor_panel_test)
endif()
