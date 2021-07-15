cmake_minimum_required(VERSION 3.10.0)

project(GDManager)

add_library(${PROJECT_NAME} SHARED 
    "src/gdm.h"     "src/core.h"
    "src/object.h"  "src/object.cpp"
    "src/file.h"    "src/file.cpp"
)

if (GDM_PRECOMPILE_HEADERS)
    target_precompile_headers(${PROJECT_NAME} PUBLIC core.h)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE GDM_BUILD_DLL)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src> 
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


file(GLOB INC_FILES "src/*.h")
install(FILES ${INC_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Config
    CONFIGURATIONS Release 
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/Release 
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/Release
)

install(TARGETS ${PROJECT_NAME} 
    CONFIGURATIONS Debug 
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/Debug
)

install(TARGETS ${PROJECT_NAME} 
    CONFIGURATIONS RelWithDebInfo 
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/RelWithDebInfo
)

install(EXPORT ${PROJECT_NAME}Config FILE ${PROJECT_NAME}Config.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)


#############################
#############################

if (GDM_PYTHON_BINDING)
    project(PyGDManager)

    add_library(${PROJECT_NAME} SHARED 
        "src/core.h"
        "src/object.h"  "src/object.cpp"
        "src/file.h"    "src/file.cpp"
        "python/pyWrapper.cpp"
    )

    if (GDM_PRECOMPILE_HEADERS)
        target_precompile_headers(${PROJECT_NAME} PUBLIC "src/core.h")
    endif()

    target_compile_definitions(${PROJECT_NAME} PRIVATE GDM_BUILD_DLL)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

    install(TARGETS ${PROJECT_NAME} 
        CONFIGURATIONS Release 
            RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/python
    )

    install(FILES "python/GDManager.py" 
        CONFIGURATIONS Release 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/python
    )
endif()

