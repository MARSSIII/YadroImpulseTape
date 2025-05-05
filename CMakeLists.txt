cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(TapeSorter
    VERSION 1.0.0
    DESCRIPTION "Tape sorting application with configurable delays"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_TESTING "Enable testing with Google Test" OFF)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

if(ENABLE_WARNINGS)
    if(MSVC)
        add_compile_options(/W4 /WX)
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

file(GLOB_RECURSE HEADERS 
    "include/factories/*.h"
    "include/entities/fileTapes/*.h"
    "include/entities/*.h"
    "include/entities/*.hpp"
    "include/interfaces/*.h"
    "include/utils/*.hpp"
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20 
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION}
)

