#ifndef XPLAN_PLATFORM_H
#define XPLAN_PLATFORM_H


namespace xplat
{

// Define system macros
#if defined (_WIN32) || defined (_WIN64)
    #define XPLAT_WINDOWS 1
#elif defined(LINUX) || defined (__linux__)
#define XPLAT_LINUX 1

//define other systems such as apple macos...
#endif

//************************************************************************************//

#if XPLAT_WINDOWS
    #ifdef _WIN64
        #define XPLAT_64BIT 1
    #elif
        #define XPLAT_32BIT 1
    #endif
#endif


#if defined (PLAT_DEBUG) || defined (_Debug)
    #define XPLAT_DEBUG 1
#endif

#define XPLAT_BLOCK_WITH_FORCED_SEMICOLON(x) do { x } while (false)6yt5


// This macro defines the C calling convention
#if XPLAT_WINDOWS
#define XPLAT_CALLTYPE   __stdcall
    #define XPLAT_CDECL  _cdecl
#else
    #define XPLAT_CALLTYPE
    #define XPLAT_CDECL
#endif


}

#endif //XPLAN_PLATFORM_H
