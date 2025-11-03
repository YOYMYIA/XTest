# third_party/cmake/PrebuiltLibraries.cmake

function(find_prebuilt_library lib_name)
    # 根据平台和架构确定路径
    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(LIB_PLATFORM_DIR "windows/x64")
        else()
            set(LIB_PLATFORM_DIR "windows/x86")
        endif()
    elseif(APPLE)
        if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64")
            set(LIB_PLATFORM_DIR "macos/arm64")
        else()
            set(LIB_PLATFORM_DIR "macos/x64")
        endif()
    else()  # Linux
        if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
            set(LIB_PLATFORM_DIR "linux/aarch64")
        else()
            set(LIB_PLATFORM_DIR "linux/x64")
        endif()
    endif()

    set(SEARCH_PATHS
            ${PROJ_TRD_DIR}/prebuilt/${LIB_PLATFORM_DIR}
            $ENV{THIRDPARTY_ROOT}/${LIB_PLATFORM_DIR}
    )

    # 查找库文件
    find_library(${lib_name}_LIBRARY
            NAMES ${lib_name} ${CMAKE_STATIC_LIBRARY_PREFIX}${lib_name}${CMAKE_STATIC_LIBRARY_SUFFIX}
            PATHS ${SEARCH_PATHS}
            PATH_SUFFIXES lib
            NO_DEFAULT_PATH
    )

    # 查找头文件
    find_path(${lib_name}_INCLUDE_DIR
            NAMES ${lib_name}.h
            PATHS ${SEARCH_PATHS}
            PATH_SUFFIXES include
            NO_DEFAULT_PATH
    )

    if(${lib_name}_LIBRARY AND ${lib_name}_INCLUDE_DIR)
        # 创建导入目标
        add_library(${lib_name} STATIC IMPORTED)
        set_target_properties(${lib_name} PROPERTIES
                IMPORTED_LOCATION ${${lib_name}_LIBRARY}
                INTERFACE_INCLUDE_DIRECTORIES ${${lib_name}_INCLUDE_DIR}
        )
        set(${lib_name}_FOUND TRUE PARENT_SCOPE)
    else()
        set(${lib_name}_FOUND FALSE PARENT_SCOPE)
    endif()
endfunction()

# 使用示例
#find_prebuilt_library(openssl)
#if(openssl_FOUND)
#    target_link_libraries(my_app PRIVATE openssl)
#else()
#    message(WARNING "OpenSSL not found, some features will be disabled")
#endif()