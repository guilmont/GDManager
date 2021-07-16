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
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
    install(TARGETS ${PROJECT_NAME} CONFIGURATIONS Release DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

    if (WIN32)
        configure_file(${PROJECT_SOURCE_DIR}/GDEditor.bat.in ${CMAKE_BINARY_DIR}/GDEditor.bat)
        install(FILES ${CMAKE_BINARY_DIR}/GDEditor.bat 
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
            CONFIGURATIONS Release DESTINATION ${CMAKE_INSTALL_PREFIX})
            
    else()
        configure_file(${PROJECT_SOURCE_DIR}/GDEditor.sh.in ${CMAKE_BINARY_DIR}/GDEditor.sh)
        install(FILES ${CMAKE_BINARY_DIR}/GDEditor.sh 
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
            CONFIGURATIONS Release DESTINATION ${CMAKE_INSTALL_PREFIX})
    endif()

endif()