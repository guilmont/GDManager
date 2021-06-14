cmake_minimum_required(VERSION 3.10.0)

project(GDManager)


file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB INC_FILES ${PROJECT_SOURCE_DIR}/*.h)

add_library(GDManager SHARED ${SRC_FILES} ${INC_FILES})
target_include_directories(GDManager PUBLIC ${PROJECT_SOURCE_DIR})
target_precompile_headers(GDManager PUBLIC ${PROJECT_SOURCE_DIR}/core.h)