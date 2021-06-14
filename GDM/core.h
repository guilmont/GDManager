#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

#include <initializer_list>
#include <memory>
#include <map>
#include <typeinfo>

#ifdef WIN32
#define GDM_API __declspec(dllexport)
#else
#define GDM_API
#endif

static void pout() { std::cout << std::endl; }

template <typename TP, typename... Args>
static void pout(TP var, Args &&...args)
{
    std::cout << var << " ";
    pout(args...);
}