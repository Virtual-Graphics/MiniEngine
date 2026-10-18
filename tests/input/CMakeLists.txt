add_executable(input_test main.cpp)
target_link_libraries(input_test PRIVATE mini_engine GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(input_test)
