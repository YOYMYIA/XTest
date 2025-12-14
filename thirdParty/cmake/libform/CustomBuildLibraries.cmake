function(build_custom_library lib_name source_dir)
    # 创建自定义构建目标
    add_custom_target(${lib_name}_build
            COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJ_TRD_DIR}/${lib_name}
            COMMAND ${CMAKE_COMMAND} -S ${source_dir} -B ${PROJ_TRD_DIR}/${lib_name}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${PROJ_LIB_DIR}
            COMMAND ${CMAKE_COMMAND} --build ${PROJ_TRD_DIR}/${lib_name} --config ${CMAKE_BUILD_TYPE}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Building ${lib_name}"
    )

    # 创建导入目标
    add_library(${lib_name} STATIC IMPORTED)
    add_dependencies(${lib_name} ${lib_name}_build)

    # 设置导入属性（根据实际库调整）
    set_target_properties(${lib_name} PROPERTIES
            IMPORTED_LOCATION ${PROJ_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib_name}${CMAKE_STATIC_LIBRARY_SUFFIX}
            INTERFACE_INCLUDE_DIRECTORIES ${PROJ_LIB_DIR}/include
    )
endfunction()

# 使用示例
#build_custom_library(mylib ${PROJECT_SOURCE_DIR}/third_party/source/some_lib)