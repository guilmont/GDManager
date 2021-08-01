#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <assert.h>

#include <filesystem>
namespace fs = std::filesystem;

#include <initializer_list>
#include <memory>
#include <unordered_map>
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

    // Some parameters to help saving to file
    inline constexpr uint32_t MAX_LABEL_SIZE = 32;                 // maximum number of characters allowed for a label
    inline constexpr uint64_t NO_DESCRIPTION = 0xffffffffffffffff; // addresss used when no description is provided to object

    inline const char *GDM_SIGNATURE = "GDManager (ver 1) by GM Oliveira";

    /////////////////////////

    static void pout() { std::cout << std::endl; }

    template <typename TP, typename... Args>
    static void pout(TP var, Args &&...args)
    {
        std::cout << var << " ";
        pout(args...);
    }

    /////////////////////////

    using Description = std::map<std::string, std::string>;

    struct Shape
    {
        uint32_t height, width;
    };

    enum class State: uint32_t
    {
        NONE,
        READ,
        WRITE
    };

    enum class Type : uint32_t
    {
        NONE = 0,
        GROUP = 1,
        INT32 = 2,
        INT64 = 3,
        UINT8 = 4,
        UINT16 = 5,
        UINT32 = 6,
        UINT64 = 7,
        FLOAT = 8,
        DOUBLE = 9,
    };

    enum class Compression : uint32_t
    {
        NONE,
        ZIP,
        LZW
    };

}