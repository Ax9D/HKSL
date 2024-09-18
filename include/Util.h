#pragma once 
#include <iostream>

#define HKSL_ERROR(message) do\
{\
    std::cout << (message) << std::endl;\
    std::exit(-1);\
} while(0)\

#define HKSL_UNREACHABLE() HKSL_ERROR("Unreachable")