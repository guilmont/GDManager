cmake_minimum_required(VERSION 3.10.0)

project(GDManager)

add_library(${PROJECT_NAME} SHARED 
    "src/GDManager.h" "src/core.h"
    "src/object.h"    "src/object.cpp"
    "src/file.h"      "src/file.cpp"
)

if (GDM_PRECOMPILE_HEADERS)
    target_precompile_headers(${PROJECT_NAME} PUBLIC "src/core.h")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE GDM_BUILD_DLL)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src> 
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME_RELEASE ${PROJECT_NAME}
    OUTPUT_NAME_RELWITHDEBINFO ${PROJECT_NAME}_RelWithDebInfo
    OUTPUT_NAME_DEBUG ${PROJECT_NAME}_Debug
)

### Installation ##########################################

file(GLOB INC_FILES "src/*.h")
install(FILES ${INC_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Config
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

install(EXPORT ${PROJECT_NAME}Config FILE ${PROJECT_NAME}Config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake)


#############################
#############################

if (GDM_PYTHON_BINDING)
    project(PyGDManager)

    add_library(${PROJECT_NAME} SHARED "python/pyWrapper.cpp")

    target_compile_definitions(${PROJECT_NAME} PRIVATE GDM_BUILD_DLL)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(${PROJECT_NAME} PUBLIC GDManager)

    install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Release
            LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python_packages
            RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python_packages
    )

    install(FILES "python/GDManager.py" CONFIGURATIONS Release DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python_packages)

endif()

