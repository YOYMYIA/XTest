//
// Created by 76426 on 2025/11/16.
//

#ifndef XPLAN_STANDARD_H
#define XPLAN_STANDARD_H


namespace xplat
{

#include "Platform.h"

#ifdef XPLAT_LINUX
// Linux
    #ifdef XPLAT_BUILD_DLL
        #define XPLAT_API __attribute__((visibility("default")))
    #else
        #define XPLAT_API
    #endif
    #define XPLAT_CALL
#else
// Windows
#ifdef DMAI_API_SHARED
    #define XPLAT_API __declspec(dllexport)
#else
    #define XPLAT_API __declspec(dllimport)
#endif
    #define XPLAT_CALL __stdcall  // WINDOWS_CALL_CONVENTIONS
#endif

#define PLAT_MAJOR_VERSION      1
#define PLAT_MINOR_VERSION      0
#define PLAT_BUILDNUMBER        0

#define PLAT_VERSION   ((PLAT_MAJOR_VERSION << 16) + (PLAT_MINOR_VERSION << 8) + PLAT_BUILDNUMBER)

#define PLAT_VERSION_ID \
    [[maybe_unused]] volatile auto platVersionId = "plat_version_" PLAT_STRINGIFY(PLAT_MAJOR_VERSION) "_" PLAT_STRINGIFY(PLAT_MINOR_VERSION) "_" PLAT_STRINGIFY(PLAT_BUILDNUMBER);



// 兼容C和C++
#ifdef __cplusplus
extern "C" {
#endif





#ifdef __cplusplus
}
#endif

}

#endif //XPLAN_STANDARD_H
