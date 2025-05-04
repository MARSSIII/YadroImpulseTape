cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(PROJECT_NAME TapeSorter) 

project(${PROJECT_NAME}
        LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/FileTape.cpp
    src/TapeSorter.cpp
    src/factories/BuilderTapeConfigFactory.cpp
    src/factories/FileTapeConfigFactory.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

