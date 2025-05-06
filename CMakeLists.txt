cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(TapeSorter
    VERSION 1.0.0
    DESCRIPTION "Tape sorting application with configurable delays"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_TESTING "Enable testing with Google Test" ON)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

if(ENABLE_WARNINGS)
    if(MSVC)
        add_compile_options(/W4)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            add_compile_options(/WX)
        endif()
    else()
        add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/factories/*.cpp"
    "src/entities/fileTapes/*.cpp"
    "src/entities/*.tpp"
    "src/utils/*.cpp"
)

# Удаляем main.cpp из списка исходников для библиотеки
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

file(GLOB_RECURSE HEADERS
    "include/factories/*.h"
    "include/entities/fileTapes/*.h"
    "include/entities/*.h"
    "include/entities/*.hpp"
    "include/interfaces/*.h"
    "include/utils/*.hpp"
)

# Создаем библиотеку из исходников (без main.cpp)
add_library(TapeSorterLib STATIC
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(TapeSorterLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Создаем исполняемый файл только из main.cpp
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE TapeSorterLib)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION}
)

if(ENABLE_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    add_subdirectory(tests)
endif()  
