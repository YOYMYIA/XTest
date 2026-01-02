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

# CUDA Toolkit
find_package(CUDAToolkit REQUIRED)
message(STATUS "CUDA Toolkit found: ${CUDAToolkit_VERSION}")

set(GTEST_DIR "${THIRD_PARTY_PREFIX}/GTest")
list(APPEND CMAKE_PREFIX_PATH ${GTEST_DIR})
find_package(GTest REQUIRED)

set(OPENCV_DIR "${THIRD_PARTY_PREFIX}/OpenCV")
list(APPEND CMAKE_PREFIX_PATH ${OPENCV_DIR})
find_package(OpenCV REQUIRED)

#find_package(Python3 COMPONENTS Interpreter Development REQUIRED)


# 添加cmake模板
list(APPEND CMAKE_MODULE_PATH "${PROJ_TRD_DIR}/cmake")
include(Manage3td)

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


# ================================================================
# 函数：copy_directory_files
# 功能：复制源目录中的文件到目标目录
# 参数：
#   SOURCE_DIR - 源目录
#   DEST_DIR   - 目标目录
#   PATTERN    - 文件匹配模式（可选，默认为 "*"）
#   RECURSE    - 是否递归子目录（可选，默认为 FALSE）
# ================================================================
function(copy_directory_files SOURCE_DIR DEST_DIR)
    # 基本参数检查
    if(NOT SOURCE_DIR)
        message(FATAL_ERROR "copy_directory_files: 缺少源目录参数")
    endif()

    if(NOT DEST_DIR)
        message(FATAL_ERROR "copy_directory_files: 缺少目标目录参数")
    endif()

    if(NOT EXISTS ${SOURCE_DIR})
        message(WARNING "copy_directory_files: 源目录不存在: ${SOURCE_DIR}")
        return()
    endif()

    # 解析可选参数
    set(PATTERN "*")
    set(RECURSE FALSE)

    # 处理额外参数
    set(EXTRA_ARGS ${ARGN})
    list(LENGTH EXTRA_ARGS EXTRA_COUNT)

    if(EXTRA_COUNT GREATER 0)
        list(GET EXTRA_ARGS 0 FIRST_ARG)
        string(TOUPPER "${FIRST_ARG}" FIRST_ARG_UPPER)

        if(FIRST_ARG_UPPER STREQUAL "RECURSE")
            set(RECURSE TRUE)
            if(EXTRA_COUNT GREATER 1)
                set(PATTERN "${EXTRA_ARGS}")
                list(REMOVE_AT EXTRA_ARGS 0)
                string(REPLACE ";" " " PATTERN "${EXTRA_ARGS}")
            endif()
        else()
            set(PATTERN "${FIRST_ARG}")
            if(EXTRA_COUNT GREATER 1)
                list(GET EXTRA_ARGS 1 SECOND_ARG)
                string(TOUPPER "${SECOND_ARG}" SECOND_ARG_UPPER)
                if(SECOND_ARG_UPPER STREQUAL "RECURSE")
                    set(RECURSE TRUE)
                endif()
            endif()
        endif()
    endif()

    # 创建目标目录（如果不存在）
    if(NOT EXISTS ${DEST_DIR})
        file(MAKE_DIRECTORY ${DEST_DIR})
        message(STATUS "创建目录: ${DEST_DIR}")
    endif()

    # 根据是否递归选择查找方式
    if(RECURSE)
        file(GLOB_RECURSE FILES_TO_COPY "${SOURCE_DIR}/${PATTERN}")
        message(STATUS "递归复制: ${SOURCE_DIR}/${PATTERN} -> ${DEST_DIR}")
    else()
        file(GLOB FILES_TO_COPY "${SOURCE_DIR}/${PATTERN}")
        message(STATUS "复制: ${SOURCE_DIR}/${PATTERN} -> ${DEST_DIR}")
    endif()

    # 过滤掉目录，只保留文件
    set(FILES_ONLY "")
    foreach(FILE_PATH ${FILES_TO_COPY})
        if(EXISTS ${FILE_PATH} AND NOT IS_DIRECTORY ${FILE_PATH})
            list(APPEND FILES_ONLY ${FILE_PATH})
        endif()
    endforeach()

    # 复制文件
    set(COPIED_COUNT 0)
    foreach(FILE_PATH ${FILES_ONLY})
        get_filename_component(FILE_NAME ${FILE_PATH} NAME)

        # 计算相对路径（用于保持目录结构）
        if(RECURSE)
            file(RELATIVE_PATH RELATIVE_PATH ${SOURCE_DIR} ${FILE_PATH})
            get_filename_component(RELATIVE_DIR ${RELATIVE_PATH} DIRECTORY)

            # 创建子目录
            if(RELATIVE_DIR)
                set(TARGET_SUBDIR "${DEST_DIR}/${RELATIVE_DIR}")
                if(NOT EXISTS ${TARGET_SUBDIR})
                    file(MAKE_DIRECTORY ${TARGET_SUBDIR})
                endif()
                set(TARGET_PATH "${TARGET_SUBDIR}/${FILE_NAME}")
            else()
                set(TARGET_PATH "${DEST_DIR}/${FILE_NAME}")
            endif()
        else()
            set(TARGET_PATH "${DEST_DIR}/${FILE_NAME}")
        endif()

        # 复制文件（仅当不同时复制）
        add_custom_command(
                OUTPUT ${TARGET_PATH}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${FILE_PATH}"
                "${TARGET_PATH}"
                DEPENDS ${FILE_PATH}
                COMMENT "复制: ${FILE_NAME}"
        )

        # 将输出文件添加到自定义目标
        list(APPEND ALL_TARGETS ${TARGET_PATH})

        math(EXPR COPIED_COUNT "${COPIED_COUNT} + 1")
    endforeach()

    # 如果没有文件，直接返回
    if(COPIED_COUNT EQUAL 0)
        message(STATUS "没有找到匹配的文件")
        return()
    endif()

    # 创建自定义目标（如果需要立即执行）
    if(NOT "${DEST_DIR}" MATCHES "^\\$<")
        # 生成目标名称
        string(MAKE_C_IDENTIFIER "copy_${SOURCE_DIR}_to_${DEST_DIR}" TARGET_NAME)

        add_custom_target(${TARGET_NAME} ALL
                DEPENDS ${ALL_TARGETS}
                COMMENT "复制文件从 ${SOURCE_DIR} 到 ${DEST_DIR}"
        )
    endif()

    message(STATUS "将复制 ${COPIED_COUNT} 个文件")
endfunction()

