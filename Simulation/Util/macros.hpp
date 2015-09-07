#ifndef MACROS_HPP
#define MACROS_HPP
#include "../globals.h"

#ifdef DEBUG_BUILD
    #define DEBUG(X) if(rank == 0) std::cerr << X << std::endl
#else
    #define DEBUG(X)
#endif

#endif // MACROS_HPP
