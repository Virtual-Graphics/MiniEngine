add_executable(ecs_test main.cpp)

target_link_libraries(ecs_test PRIVATE vulcal_engine GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ecs_test)
