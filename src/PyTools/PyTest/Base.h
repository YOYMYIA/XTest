//
// Created by 76426 on 2025/10/29.
//

#ifndef XVISION_BASE_H
#define XVISION_BASE_H

#ifdef PLAT_LINUX
// Linux 平台
#ifdef TEST_BUILD_DLL
#define TEST_API __attribute__((visibility("default")))
#else
#define TEST_API
#endif
#define TEST_CALL
#else
// Windows 平台
#ifdef DMAI_API_SHARED
#define TEST_API __declspec(dllexport)
#else
#define TEST_API __declspec(dllimport)
#endif
#define TEST_CALL __stdcall // Windows调用约定
#endif

// 兼容C和C++
#ifdef __cplusplus
extern "C"
{
#endif
    TEST_API int add(int a, int b);
#ifdef __cplusplus
}
#endif

#endif // XVISION_BASE_H
