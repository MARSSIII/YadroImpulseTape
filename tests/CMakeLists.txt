cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(TapeSorterTests LANGUAGES CXX)

file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    TapeSorterLib
    GTest::gtest
    GTest::gtest_main
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME} 
        --gtest_verbose=1 
        --gtest_color=yes
        --gtest_print_time=1
)

