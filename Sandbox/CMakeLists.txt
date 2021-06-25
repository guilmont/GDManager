cmake_minimum_required(VERSION 3.10.0)

project(Sandbox)

add_executable(Sandbox sandbox.cpp)
target_link_libraries(Sandbox PRIVATE GDManager)


# A quick API to see the hierarchical structural of GDM files from the command line
add_executable(ViewTree viewTree.cpp)
target_link_libraries(ViewTree PRIVATE GDManager)
