cmake_minimum_required(VERSION 3.15)

set(XTEST_VERSION 1.0.0)
set(GIT_API_URL "https://api.github.com/YOYMYIA/XTest")
#定义项目名称,设置项目版本为变量 指定项目语言为
project(XTest VERSION ${XTEST_VERSION})
#定义项目名称为 XTest
set(PROJECT_NAME_CAPITALIZED "XTest")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

# 设置C/C++标准
# 启动对C17标准的支持
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#设置文件路径
set(PROJ_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(PROJ_SRC_DIR ${PROJ_ROOT_DIR}/src)
set(PROJ_TRD_DIR ${PROJ_ROOT_DIR}/thirdParty)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#获取git上的hash值
execute_process(
                COMMAND git log -1 --format=%h
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "GIT_HASH:${GIT_HASH}")

# 将GIT_HASH传递给源代码
add_definitions(-DGIT_HASH="${GIT_HASH}")

# DMAI_API_SHARED 用于控制导出，所有平台都定义
add_definitions(-DDMAI_API_SHARED)

# ============================================================================
# 选项配置
# ============================================================================

option(BUILD_SHARED_LIBS "Build as shared libraries" ON)
option(ENABLE_LARGEFILE "Enable large file support" ON)
option(ENABLE_LOGGING "Enable logging system" ON)


if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(PROJ_ARCH "x64")
else ()
  set(PROJ_ARCH "x86")
  add_definitions(-DPLAT_LINUX)
endif ()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(PROJ_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/build/build-${PROJ_ARCH}-Debug")
  if(${PROJ_ARCH} STREQUAL "x86")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -rdynamic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
  endif ()
else ()
  set(PROJ_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/build/build-${PROJ_ARCH}-Release")
endif()

set(PROJ_BIN_DIR "${PROJ_OUTPUT_DIR}/bin")
set(PROJ_RLUGIN_DIR "${PROJ_OUTPUT_DIR}/lib/plugins")

if(${PROJ_ARCH} STREQUAL "x64")
  set(PROJ_LIB_DIR "${PROJ_OUTPUT_DIR}/bin")
else ()
  set(PROJ_LIB_DIR "${PROJ_OUTPUT_DIR}/lib")
endif ()

# 添加cmake模板
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Utility)
include(ManageThirdPartyLibrary)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-DPALT_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL Release)
else()
  set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -Wall -g -ggdb -Wno-psabi")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -g -ggdb -Wno-psabi")
endif ()

#这是AddressSanitizer（ASan）的一部分，用于检测内存泄漏。
#在程序退出时，它会报告哪些内存块被分配了但没有被释放。
#if(WITH_CMAKE_LEAK)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fasnitize=leak")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fasnitize=leak")
#endif ()
#if(WITH_CMAKE_ADDRESS)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fasnitize=address")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fasnitize=address")
#endif ()
#一个调试相关的选项。它告诉编译器不要省略帧指针 编译器可能会省略帧指针以节省一个寄存器（通常用于提高性能）。
#if(WITH_CMAKE_POINIER)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
#endif ()

#set(CMAKE_C_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -Wno-psabi")
#set(CMAKE_CXX_FLAGS_DEBUG  "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -Wno-psabi")



#放入全局目录，链接全局库文件（库）
INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Base
)

LINK_DIRECTORIES(

)

#主程序构建
add_subdirectory(src)


# 可选：添加示例或测试目录
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples)
  add_subdirectory(examples)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
  add_subdirectory(tests)
endif()