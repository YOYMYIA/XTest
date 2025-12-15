# 设置Python版本
set(PYBIND11_PYTHON_VERSION 3.11)
# set(Python_VIRTUALENV FIRST) # 如果需要虚拟环境则取消注释

# 查找Python，指定版本
find_package(Python ${PYBIND11_PYTHON_VERSION} EXACT REQUIRED COMPONENTS Interpreter Development)

# 设置pybind11路径（如果未安装在标准位置）
if(NOT DEFINED PUBIND11_DIR)
    set(PUBIND11_DIR ${THIRD_PARTY_PREFIX}/pybind11)
    list(APPEND CMAKE_PREFIX_PATH ${PUBIND11_DIR})
endif()

# 查找pybind11库
find_package(pybind11 CONFIG REQUIRED)

# 收集源文件：只收集cpp文件，避免头文件被错误地加入

file(GLOB_RECURSE CPP_FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h" 
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    )
    file(GLOB_RECURSE CPP_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c" 
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    )

if(CPP_FILES)
    set(CPP_SOURCES ${CPP_FILES})
    message(STATUS "Found CPP files: ${CPP_SOURCES}")
else()
    message(FATAL_ERROR "No .cpp source files found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# 确保PY_PKG变量被正确定义，如果没有定义，则设置一个默认值
if(NOT DEFINED PY_PKG)
    set(PY_PKG "pymodule")   # 设置一个默认模块名，或者根据实际情况修改
endif()

# 确保模块名不包含非法字符
string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" SAFE_MODULE_NAME "${PY_PKG}")
if(NOT "${PY_PKG}" STREQUAL "${SAFE_MODULE_NAME}")
    message(WARNING "Module name '${PY_PKG}' contains invalid characters. Using '${SAFE_MODULE_NAME}' instead.")
    set(PY_PKG "${SAFE_MODULE_NAME}")
endif()

# 创建Python模块：第一个参数是模块名，后面是源文件
pybind11_add_module(${PY_PKG} ${CPP_FILES})

# 包含目录
target_include_directories(${PY_PKG} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Python_INCLUDE_DIRS}
    ${pybind11_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(${PY_PKG} PRIVATE
    pybind11::module
    ${Python_LIBRARIES}
)

# 创建输出目录
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../output/")

# 复制生成的文件到指定位置
add_custom_command(
    TARGET ${PY_PKG} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${PY_PKG}>"
        "${CMAKE_CURRENT_SOURCE_DIR}/../output/"
    COMMENT "Copying ${PY_PKG} to output directory"
)
# 复制生成的文件到指定位置
# add_custom_command(
#     TARGET ${PY_PKG} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "$<TARGET_FILE:${PY_PKG}>"
#     "${CMAKE_CURRENT_SOURCE_DIR}/../output/${PY_PKG}$<TARGET_FILE_EXTENSION:${PY_PKG}>"
#     COMMENT "Copying ${PY_PKG} to output directory"
# )