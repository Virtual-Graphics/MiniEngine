add_executable(editor_state_test
    main.cpp
    ../../apps/editor/editor_context.cpp
    ../../apps/editor/editor_camera.cpp
)
target_include_directories(editor_state_test PRIVATE ../../apps)
target_link_libraries(editor_state_test PRIVATE mini_engine GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(editor_state_test)
