cmake_minimum_required(VERSION 3.10.0)

project(GDManager)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB INC_FILES ${PROJECT_SOURCE_DIR}/*.h)

add_library(GDManager SHARED ${SRC_FILES} ${INC_FILES})
target_precompile_headers(GDManager PUBLIC ${PROJECT_SOURCE_DIR}/core.h)
target_include_directories(GDManager PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_definitions(GDManager PRIVATE GDM_BUILD_DLL)