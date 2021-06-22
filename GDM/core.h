#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

#include <filesystem>
namespace fs = std::filesystem;


#include <initializer_list>
#include <memory>
#include <map>
#include <typeinfo>

#ifdef WIN32
    #ifdef GDM_BUILD_DLL
        #define GDM_API __declspec(dllexport)
    #else
        #define GDM_API __declspec(dllimport)
    #endif
#else
    #define GDM_API
#endif



///////////////////////////////////////////////////////////////////////////////
// SOME DEFINITIONS WE WILL NEED EVERYWHERE

namespace GDM
{

    constexpr uint32_t MAX_LABEL_SIZE = 64;

    static void pout() { std::cout << std::endl; }

    template <typename TP, typename... Args>
    static void pout(TP var, Args &&...args)
    {
        std::cout << var << " ";
        pout(args...);
    }

    /////////////////////////

    struct Shape
    {
        uint32_t width, height;
    };

    enum class Type : uint32_t
    {
        GROUP,
        INT32,
        UINT8,
        UINT16,
        UINT32,
        FLOAT,
        DOUBLE,
        STRING,
    };

}