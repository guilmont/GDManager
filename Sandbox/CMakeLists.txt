cmake_minimum_required(VERSION 3.10.0)

if (GDM_TEST)
    project(GDMTest)
    add_executable(${PROJECT_NAME} sandbox.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE GDManager)

    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib/Release)


    install(TARGETS ${PROJECT_NAME} 
        CONFIGURATIONS Release 
            DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()


if (GDM_CMD_VIEWER)
    project(GDMTree)
    add_executable(${PROJECT_NAME} viewTree.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE GDManager)

    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib/Release)

    install(TARGETS ${PROJECT_NAME} 
        CONFIGURATIONS Release 
            DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()
