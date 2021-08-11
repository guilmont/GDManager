cmake_minimum_required(VERSION 3.10.0)

if (GDM_EDITOR)
    set(ASSETS_DIR "assets/GDEditor")
    
    add_subdirectory(vendor/GRender/GRender)

    project(GDEditor)
    add_executable(${PROJECT_NAME} 
        "src/main.cpp"
        "src/gdeditor.h"  "src/gdeditor.cpp"
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE GDManager GRender)
    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        INSTALL_PATH="${CMAKE_INSTALL_PREFIX}"
        ASSETS="${ASSETS_DIR}"
    )

    install(TARGETS ${PROJECT_NAME} 
        CONFIGURATIONS Release
            DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    )

    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_INSTALL_PREFIX})

endif()