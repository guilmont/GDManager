cmake_minimum_required(VERSION 3.10.0)

if (GDM_EDITOR)
    set(GRENDER_TEMPLATE OFF CACHE BOOL "Turn off GRender template")
    add_subdirectory(vendor/GRender)

    project(GDEditor)
    add_executable(${PROJECT_NAME} 
        "src/main.cpp"
        "src/gdeditor.h"  "src/gdeditor.cpp"
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE GDManager GRender)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS  INSTALL_PATH="${CMAKE_INSTALL_PREFIX}/bin")
    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
    install(TARGETS ${PROJECT_NAME} CONFIGURATIONS Release DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

endif()