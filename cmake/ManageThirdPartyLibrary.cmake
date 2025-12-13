# 平台检测和系统库配置
if(PROJ_ARCH MATCHES "x64" OR PROJ_ARCH MATCHES "win64")
    # Windows 平台
    message(STATUS "Configuring for Windows x64")

    set(SYSTEM_LIBS
            mingwex  # 放在前面解决符号冲突
            #setupapic
            ws2_32
            mswsock
            advapi32
            user32
            kernel32
    )
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_FLAGS} -lmingwex -lmsvcrt")

    # 允许重复定义解决 _stat64i32 冲突
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-multiple-definition")

else()
    # Linux/Unix 平台
    message(STATUS "Configuring for Unix/Linux")

    set(SYSTEM_LIBS
            pthread
            stdc++fs  # C++17文件系统库
            stdc++
            m
            dl
            rt
    )
endif()

# 第三方库配置
set(THIRD_PARTY_PREFIX "${PROJ_TRD_DIR}/${PROJ_ARCH}")
set(GTEST_LIBS gtest gmock)

# 配置选项
option(ENABLE_QT "Enable Qt support" OFF)
option(ENABLE_GTEST "Enable Google Test" ON)
option(ENABLE_GFLAGS "Enable gflags library" OFF)
option(ENABLE_GLOG "Enable glog library" OFF)
option(BUILD_THIRD_PARTY_GTEST "Build gtest" ON)

# 平台特定的第三方库配置
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "Windows platform detected")

    # Windows 特定变量
    set(PLATFORM_DEPENDENT_LIBS "")

    if(ENABLE_QT)
        message(STATUS "Qt support enabled - remember to set QT5_DIR")
    endif()

    if(ENABLE_GTEST)
        message(STATUS "GTest enabled - remember to set GTEST_DIR")
    endif()

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Linux platform detected")

    # Linux 特定变量
    set(PLATFORM_DEPENDENT_LIBS pthread dl)

    if(ENABLE_QT)
        message(STATUS "Qt support enabled - remember to set QT5_DIR")
    endif()

    if(ENABLE_GTEST)
        message(STATUS "GTest enabled - remember to set GTest_DIR")
    endif()

else()
    message(STATUS "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Qt5 配置
if(ENABLE_QT)
    # 查找Qt5组件
    find_package(Qt5 REQUIRED COMPONENTS
            Core
            Widgets
            Gui
            Charts
            PrintSupport
            OpenGL
            Xml
            Network
            UiTools
    )

    # 定义QT5_LIBS变量
    set(QT5_LIBS
            Qt5::Core
            Qt5::Widgets
            Qt5::Gui
            Qt5::Charts
            Qt5::PrintSupport
            Qt5::OpenGL
            Qt5::Xml
            Qt5::Network
            Qt5::UiTools
    )

    # 添加Qt5自动处理
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    message(STATUS "Qt5 support enabled")
endif()

# 第三方库构建
#list(APPEND CMAKE_MODULE_PATH "${PROJ_TRD_DIR}/cmake")
#include(ThirdPartyBuild)

set(GTEST_DIR "${THIRD_PARTY_PREFIX}/GTest")
list(APPEND CMAKE_PREFIX_PATH ${GTEST_DIR})
find_package(GTest REQUIRED)

set(OPENCV_DIR "${THIRD_PARTY_PREFIX}/OpenCV")
list(APPEND CMAKE_PREFIX_PATH ${OPENCV_DIR})
find_package(OpenCV REQUIRED)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

#set(PYBIND11_DIR "${THIRD_PARTY_PREFIX}/pybind11")
#list(APPEND CMAKE_PREFIX_PATH ${PYBIND11_DIR})
#find_package(pybind11 REQUIRED)

#file(GLOB GTEST_BINARY "${GTEST_DIR}/bin/*")
#file(COPY ${GTEST_BINARY} DESTINATION "${EXECUTABLE_OUTPUT_PATH}/")

#set(GTEST_DIR "${THIRD_PARTY_PREFIX}/GTest")
#list(APPEND CMAKE_MODULE_PATH ${GTEST_DIR}/lib)
#find_package(GTest REQUIRED)
#link_directories(${GTEST_DIR}/lib)
#include_directories(${GTEST_DIR}/include)

# 库存在性检查函数
function(check_libraries_exist LIB_LIST)
    if(NOT ${LIB_LIST})
        return()
    endif()

    foreach(LIB ${${LIB_LIST}})
        if(EXISTS "${LIB}" OR NOT LIB MATCHES ".*/.*")  # 如果是完整路径则检查存在性
            message(STATUS "Library ${LIB} - found")
        else()
            message(WARNING "Library ${LIB} - not found")
        endif()
    endforeach()
endfunction()
