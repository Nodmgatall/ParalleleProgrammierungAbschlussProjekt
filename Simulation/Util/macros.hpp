#ifndef MACROS_HPP
#define MACROS_HPP

#ifdef DEBUG_BUILD
    #define DEBUG(X) std::cerr << X << std::endl
#else
    #define DEBUG(X)
#endif

#endif // MACROS_HPP
