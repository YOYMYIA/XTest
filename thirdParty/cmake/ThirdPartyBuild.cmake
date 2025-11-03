# 第三方库构建核心函数

# 构建提供CMake的库
function(build_cmake_library lib_name source_dir)
    # 添加子目录
    add_subdirectory(${source_dir} ${CMAKE_BINARY_DIR}/thirdparty/${lib_name})
endfunction()


## 构建自定义库（无CMake或需要特殊处理）
#function(build_custom_library lib_name source_dir target_name)
#    # 检查源文件
#    if(NOT EXISTS ${source_dir})
#        message(WARNING "Source directory not found: ${source_dir}")
#        return()
#    endif()
#
#    # 查找源文件
#    file(GLOB_RECURSE lib_sources
#            ${source_dir}/*.cpp
#            ${source_dir}/*.c
#            ${source_dir}/*.cc
#    )
#
#    if(NOT lib_sources)
#        message(WARNING "No source files found for ${lib_name}")
#        return()
#    endif()
#
#    # 查找头文件
#    file(GLOB_RECURSE lib_headers
#            ${source_dir}/*.h
#            ${source_dir}/*.hpp
#            ${source_dir}/*.hxx
#    )
#
#    # 创建库目标
#    add_library(${target_name} STATIC ${lib_sources} ${lib_headers})
#
#    # 设置包含目录
#    if(EXISTS ${source_dir}/include)
#        target_include_directories(${target_name}
#                PUBLIC ${source_dir}/include
#        )
#    else()
#        target_include_directories(${target_name}
#                PUBLIC ${source_dir}
#        )
#    endif()
#
#    # 设置编译选项
#    target_compile_features(${target_name} PRIVATE cxx_std_17)
#    target_compile_definitions(${target_name} PRIVATE
#            ${lib_name}_BUILD
#            ${lib_name}_STATIC
#    )
#
#    # 设置输出属性
#    set_target_properties(${target_name} PROPERTIES
#            ARCHIVE_OUTPUT_DIRECTORY ${THIRD_PARTY_ARCHIVE_OUTPUT}
#            LIBRARY_OUTPUT_DIRECTORY ${THIRD_PARTY_LIBRARY_OUTPUT}
#            RUNTIME_OUTPUT_DIRECTORY ${THIRD_PARTY_RUNTIME_OUTPUT}
#            OUTPUT_NAME ${lib_name}
#    )
#
#    message(STATUS "Built custom library: ${target_name}")
#endfunction()
#
## 创建仅头文件库目标
#function(create_header_only_targets)
#    # Catch2
#    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/headers/catch2/include)
#        add_library(third_party_catch2 INTERFACE)
#        target_include_directories(third_party_catch2 INTERFACE
#                ${CMAKE_CURRENT_SOURCE_DIR}/headers/catch2/include
#        )
#        message(STATUS "Created header-only target: third_party_catch2")
#    endif()
#
#    # nlohmann_json
#    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/headers/nlohmann_json/include)
#        add_library(third_party_nlohmann_json INTERFACE)
#        target_include_directories(third_party_nlohmann_json INTERFACE
#                ${CMAKE_CURRENT_SOURCE_DIR}/headers/nlohmann_json/include
#        )
#        message(STATUS "Created header-only target: third_party_nlohmann_json")
#    endif()
#endfunction()
#
## 预编译库处理
#function(add_prebuilt_library target_name lib_path include_path)
#    if(NOT EXISTS ${lib_path})
#        message(WARNING "Prebuilt library not found: ${lib_path}")
#        return()
#    endif()
#
#    if(NOT EXISTS ${include_path})
#        message(WARNING "Include path not found: ${include_path}")
#        return()
#    endif()
#
#    add_library(${target_name} STATIC IMPORTED)
#    set_target_properties(${target_name} PROPERTIES
#            IMPORTED_LOCATION ${lib_path}
#            INTERFACE_INCLUDE_DIRECTORIES ${include_path}
#    )
#
#    message(STATUS "Added prebuilt library: ${target_name}")
#endfunction()