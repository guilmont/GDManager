cmake_minimum_required(VERSION 3.10.0)

project(GDManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Setup runtime output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

option(BUILD_TESTS "Builds tests on GDManager API" ON)
option(BUILD_EDITOR "Builds graphical interface to interact with GDM files" ON)


# Building things accordingly
add_subdirectory(Manager)

if (BUILD_TESTS)
    add_subdirectory(Sandbox)
endif()

if (BUILD_EDITOR)
    add_subdirectory(Editor)
endif()