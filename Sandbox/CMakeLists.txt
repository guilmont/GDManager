cmake_minimum_required(VERSION 3.10.0)

if (GDM_TEST)
    project(Sandbox)
    add_executable(Sandbox sandbox.cpp)
    target_link_libraries(Sandbox PRIVATE GDManager)

    install(TARGETS Sandbox DESTINATION .)
endif()


if (GDM_CMD_VIEWER)
    project(Viewer)
    add_executable(ViewTree viewTree.cpp)
    target_link_libraries(ViewTree PRIVATE GDManager)

    install(TARGETS ViewTree DESTINATION .)
endif()
