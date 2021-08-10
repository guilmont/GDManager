cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Setup build output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "Location to install this application")

option(GDM_PYTHON_BINDING "Build python buildings got GDManager" ON)
option(GDM_EDITOR "Build graphical interface to interact with GDM files" ON)
option(GDM_CMD_VIEWER "Build command line tool to GDM file structure" ON)
option(GDM_TEST "Builds file template for testing" OFF)
option(GDM_PRECOMPILE_HEADERS "Precompiles GDM headers" OFF)

#######################################
project(GDManager)

add_subdirectory(Manager)
add_subdirectory(Sandbox)
add_subdirectory(Editor)
