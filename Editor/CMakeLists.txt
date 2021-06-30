cmake_minimum_required(VERSION 3.10.0)

add_subdirectory(vendor/GRender)

project(GDEditor)

add_executable(GDEditor 
    "src/main.cpp"
    "src/gdeditor.h"  "src/gdeditor.cpp"
)

target_link_libraries(GDEditor PRIVATE GDManager GRender)

target_compile_definitions(GDEditor PRIVATE _CRT_SECURE_NO_WARNINGS)