#pragma once 
#include <iostream>

#define HKSL_ERROR(message) do\
{\
    std::cout << (message) << std::endl;\
    std::exit(-1);\
} while(0)

#define HKSL_UNREACHABLE() do\
{\
    std::cout << "Reached unreachable code in " << __FILE__ << ":" << __LINE__ << std::endl; \
    std::exit(-1);\
} while(0)

#define HKSL_TODO() do\
{\
    std::cout << "Unimplemented code in " << __FILE__ << ":" << __LINE__ << std::endl; \
    std::exit(-1);\
} while(0)