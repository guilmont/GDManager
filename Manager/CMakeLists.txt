cmake_minimum_required(VERSION 3.10.0)

project(GDManager)

add_library(GDManager SHARED 
    "gdm.h"     "core.h"
    "object.h"  "object.cpp"
    "file.h"    "file.cpp"
)

if (GDM_PRECOMPILE_HEADERS)
    target_precompile_headers(GDManager PUBLIC core.h)
endif()

target_include_directories(GDManager PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_definitions(GDManager PRIVATE GDM_BUILD_DLL)


file(GLOB INC_FILES "*.h")
install(FILES ${INC_FILES} DESTINATION include)
install(TARGETS GDManager DESTINATION .)

#############################
#############################

if (GDM_PYTHON_BINDING)
    add_library(PyManager SHARED 
        "gdm.h"     "core.h"
        "object.h"  "object.cpp"
        "file.h"    "file.cpp"
        "pyWrapper.cpp"
    )

    if (GDM_PRECOMPILE_HEADERS)
        target_precompile_headers(GDManager PUBLIC core.h)
    endif()

    target_compile_definitions(PyManager PRIVATE GDM_BUILD_DLL)

    file(COPY "GDManager.py" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    install(FILES "GDManager.py" DESTINATION python_package)
    install(TARGETS PyManager DESTINATION python_package)
endif()