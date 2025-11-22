SET(EXECUTABLE_OUTPUT_PATH "${PROJ_BIN_DIR}")
SET(LIBRARY_OUTPUT_PATH "${PROJ_LIB_DIR}")

if(NOT EXISTS "${PROJ_OUTPUT_DIR}/conf/")
    file(GLOB PROJ_CONFIG_FINE "${PROJ_ROOT_DIR}/conf/*")
    file(COPY ${PROJ_CONFIG_FINE} DESTINATION "${PROJ_OUTPUT_DIR}/conf/")
else()
    message("Config has already exits")
endif ()

macro(add_add_subdirs)
    file(GLOB dirs "*")
    foreach (dir IN LISTS dirs)
        if(IS_DIRECTORY ${dir} AND EXISTS ${dir}/CMakeLists.txt)
            add_subdirectory("${dir}")
        endif ()
    endforeach ()
endmacro()

# 增强的宏，自动处理所有子目录，包括特殊目录
macro(add_add_subdirs_ignore_test)
    file(GLOB dirs "*")
    foreach (dir IN LISTS dirs)
        if(IS_DIRECTORY ${dir} AND EXISTS ${dir}/CMakeLists.txt)
            # 跳过特殊目录，因为它们已经在build_shared_library中处理了
            if(NOT "${dir}" MATCHES "/test$" AND 
               NOT "${dir}" MATCHES "/docs$" AND 
               NOT "${dir}" MATCHES "/example$")
                message(STATUS "Adding subdirectory: ${dir}")
                add_subdirectory("${dir}")
            else()
                message(STATUS "Skipping special directory: ${dir}")
            endif()
        endif ()
    endforeach ()
endmacro()


function(build_shared_library LIB_NAME)
    set(${LIB_NAME}_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE ${LIB_NAME}_header "${${LIB_NAME}_DIR}/*.h" "${${LIB_NAME}_DIR}/*.hpp")
    file(GLOB_RECURSE ${LIB_NAME}_src "${${LIB_NAME}_DIR}/*.c" "${${LIB_NAME}_DIR}/*.cpp")
    file(GLOB_RECURSE ${LIB_NAME}_cc "${${LIB_NAME}_DIR}/*.cc")
    file(GLOB_RECURSE ${LIB_NAME}_cxx "${${LIB_NAME}_DIR}/*.cxx")

    set(all_sources ${${LIB_NAME}_src} ${${LIB_NAME}_cc} ${${LIB_NAME}_cxx})

    if(all_sources)
        # 有源文件，创建共享库
        add_library(${LIB_NAME} SHARED ${all_sources} ${${LIB_NAME}_header})
        target_link_libraries(${LIB_NAME} PRIVATE ${SYSTEM_LIBS} ${ARGN})
    else()
        # 只有头文件，创建接口库
        add_library(${LIB_NAME} INTERFACE)
        target_include_directories(${LIB_NAME} INTERFACE ${${LIB_NAME}_DIR})
        if(ARGN)
            target_link_libraries(${LIB_NAME} INTERFACE ${ARGN})
        endif()
    endif()
endfunction()


# 查找所有子目录中的test目录并构建测试
function(find_all_test_directories LIB_NAME SEARCH_DIR)
    # 查找当前目录下的test目录
    if(EXISTS "${SEARCH_DIR}/test" AND IS_DIRECTORY "${SEARCH_DIR}/test")
        message(STATUS "Found test directory in ${SEARCH_DIR}")
        build_tests_from_directory(${LIB_NAME} "${SEARCH_DIR}/test")
    endif()
    
    # 递归查找子目录中的test目录
    file(GLOB subdirs LIST_DIRECTORIES true "${SEARCH_DIR}/*")
    foreach(subdir IN LISTS subdirs)
        if(IS_DIRECTORY ${subdir} AND 
           NOT "${subdir}" MATCHES "/test$" AND 
           NOT "${subdir}" MATCHES "/docs$" AND 
           NOT "${subdir}" MATCHES "/example$" AND
           NOT "${subdir}" MATCHES "/build$" AND
           NOT "${subdir}" MATCHES "/\\.git$")
            find_all_test_directories(${LIB_NAME} "${subdir}")
        endif()
    endforeach()
endfunction()

# 从指定目录构建测试
function(build_tests_from_directory LIB_NAME TEST_DIR)
    # 查找测试目录下的所有源文件
    file(GLOB_RECURSE test_sources 
        "${TEST_DIR}/*.c"
        "${TEST_DIR}/*.cpp" 
        "${TEST_DIR}/*.cc"
        "${TEST_DIR}/*.cxx"
    )
    
    # 为每个测试源文件创建独立的可执行文件
    foreach(test_source ${test_sources})
        get_filename_component(test_name ${test_source} NAME_WE)
        
        # 获取相对于库目录的路径，用于生成唯一的测试名称
        file(RELATIVE_PATH rel_path ${${LIB_NAME}_DIR} ${test_source})
        string(REPLACE "/" "_" unique_suffix ${rel_path})
        string(REPLACE "\\" "_" unique_suffix ${unique_suffix})
        string(REPLACE "." "_" unique_suffix ${unique_suffix})
        
        set(test_exe_name "test_${LIB_NAME}_${unique_suffix}")
        
        message(STATUS "Adding test: ${test_exe_name} from ${rel_path}")
        
        add_executable(${test_exe_name} ${test_source})
        set_target_properties(${test_exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/../test/)
        
        # 链接主库和依赖项
        if(TARGET ${LIB_NAME})
            target_link_libraries(${test_exe_name} PRIVATE ${LIB_NAME} ${SYSTEM_LIBS} ${ARGN})
        else()
            # 对于接口库，需要手动添加包含目录
            target_include_directories(${test_exe_name} PRIVATE ${${LIB_NAME}_DIR})
            if(ARGN)
                target_link_libraries(${test_exe_name} PRIVATE ${ARGN})
            endif()
        endif()
        
        # 添加测试到CTest
        if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR BUILD_TESTING)
            add_test(NAME ${test_exe_name} COMMAND ${test_exe_name})
        endif()
    endforeach()
endfunction()

function(build_shared_library_with_test LIB_NAME)
    set(${LIB_NAME}_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    
    # 查找源文件，排除特殊目录
    file(GLOB_RECURSE ${LIB_NAME}_header 
        "${${LIB_NAME}_DIR}/*.h" 
        "${${LIB_NAME}_DIR}/*.hpp"
    )
    file(GLOB_RECURSE ${LIB_NAME}_src 
        "${${LIB_NAME}_DIR}/*.c" 
        "${${LIB_NAME}_DIR}/*.cpp"
    )
    file(GLOB_RECURSE ${LIB_NAME}_cc "${${LIB_NAME}_DIR}/*.cc")
    file(GLOB_RECURSE ${LIB_NAME}_cxx "${${LIB_NAME}_DIR}/*.cxx")

    # 过滤掉特殊目录中的源文件
    set(all_sources)
    foreach(source_file ${${LIB_NAME}_src} ${${LIB_NAME}_cc} ${${LIB_NAME}_cxx})
        string(FIND "${source_file}" "/test/" test_pos)
        string(FIND "${source_file}" "/docs/" docs_pos)
        string(FIND "${source_file}" "/example/" example_pos)
        if(test_pos EQUAL -1 AND docs_pos EQUAL -1 AND example_pos EQUAL -1)
            list(APPEND all_sources "${source_file}")
        endif()
    endforeach()

    # 检查特殊目录是否存在
    set(has_test_dir FALSE)
    set(has_docs_dir FALSE) 
    set(has_example_dir FALSE)
    
    if(EXISTS "${${LIB_NAME}_DIR}/test")
        set(has_test_dir TRUE)
    endif()
    if(EXISTS "${${LIB_NAME}_DIR}/docs")
        set(has_docs_dir TRUE)
    endif()
    if(EXISTS "${${LIB_NAME}_DIR}/example")
        set(has_example_dir TRUE)
    endif()

    if(all_sources)
        # 有源文件，创建共享库
        add_library(${LIB_NAME} SHARED ${all_sources} ${${LIB_NAME}_header})
        target_link_libraries(${LIB_NAME} PRIVATE ${SYSTEM_LIBS} ${ARGN})
        
        # 设置库的特殊目录属性
        set_target_properties(${LIB_NAME} PROPERTIES
            HAS_TEST_DIR ${has_test_dir}
            HAS_DOCS_DIR ${has_docs_dir}
            HAS_EXAMPLE_DIR ${has_example_dir}
        )
        
        # 如果有测试目录，添加测试相关配置
        if(has_test_dir)
            message(STATUS "Library ${LIB_NAME} has test directory")
            # 这里可以添加测试相关的配置，比如：
            # enable_testing()
            # add_subdirectory(test)
        endif()
        
        # 如果有文档目录，添加文档相关配置
        if(has_docs_dir)
            message(STATUS "Library ${LIB_NAME} has documentation directory")
            # 这里可以添加文档生成相关的配置
        endif()
        
        # 如果有示例目录，添加示例相关配置
        if(has_example_dir)
            message(STATUS "Library ${LIB_NAME} has example directory")
            # 这里可以添加示例构建相关的配置
        endif()
        
    else()
        # 只有头文件，创建接口库
        add_library(${LIB_NAME} INTERFACE)
        target_include_directories(${LIB_NAME} INTERFACE ${${LIB_NAME}_DIR})
        if(ARGN)
            target_link_libraries(${LIB_NAME} INTERFACE ${ARGN})
        endif()
        
        # 设置库的特殊目录属性
        set_target_properties(${LIB_NAME} PROPERTIES
            HAS_TEST_DIR ${has_test_dir}
            HAS_DOCS_DIR ${has_docs_dir}
            HAS_EXAMPLE_DIR ${has_example_dir}
        )
        
        # 特殊目录提示（对于接口库）
        if(has_test_dir)
            message(STATUS "Interface library ${LIB_NAME} has test directory")
        endif()
        if(has_docs_dir)
            message(STATUS "Interface library ${LIB_NAME} has documentation directory")
        endif()
        if(has_example_dir)
            message(STATUS "Interface library ${LIB_NAME} has example directory")
        endif()
    endif()

    # 处理测试目录 - 递归查找所有子目录中的test目录
    find_all_test_directories(${LIB_NAME} ${${LIB_NAME}_DIR})
    
    # 处理示例目录
    if(has_example_dir)
        message(STATUS "Library ${LIB_NAME} has example directory")
        
        # 使用宏来处理示例目录中的子目录
        if(EXISTS "${${LIB_NAME}_DIR}/example/CMakeLists.txt")
            add_subdirectory("${${LIB_NAME}_DIR}/example")
        else()
            # 如果没有CMakeLists.txt，自动添加所有包含CMakeLists.txt的子目录
            set(EXAMPLE_DIR "${${LIB_NAME}_DIR}/example")
            file(GLOB example_dirs LIST_DIRECTORIES true "${EXAMPLE_DIR}/*")
            foreach(example_dir IN LISTS example_dirs)
                if(IS_DIRECTORY ${example_dir} AND EXISTS ${example_dir}/CMakeLists.txt)
                    add_subdirectory("${example_dir}")
                endif()
            endforeach()
        endif()
    endif()
    
    # 处理文档目录
    if(has_docs_dir)
        message(STATUS "Library ${LIB_NAME} has documentation directory")
        # 这里可以添加文档生成逻辑，比如Doxygen配置等
    endif()
    
    # 将特殊目录信息提升到父作用域，供外部使用
    set(${LIB_NAME}_HAS_TEST ${has_test_dir} PARENT_SCOPE)
    set(${LIB_NAME}_HAS_DOCS ${has_docs_dir} PARENT_SCOPE)
    set(${LIB_NAME}_HAS_EXAMPLE ${has_example_dir} PARENT_SCOPE)
endfunction()
    

function(build_tool TOOL_NAME)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    set(${TOOL_NAME}_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE ${TOOL_NAME}_header "${${TOOL_NAME}_DIR}/*.h" "${${TOOL_NAME}_DIR}/*.hpp")
    file(GLOB_RECURSE ${TOOL_NAME}_src "${${TOOL_NAME}_DIR}/*.c" "${${TOOL_NAME}_DIR}/*.cpp")
    file(GLOB TOOL_UIS "${${TOOL_NAME}_DIR}/*.ui")
    file(GLOB ${TOOL_NAME}_qrc "${${TOOL_NAME}_DIR}/*.qrc")
    qt5_wrap_ui(ui_FILES ${TOOL_UIS})
    qt5_add_resources(qrc_FILES ${${TOOL_NAME}_qrc})
    message(STATUS ${${TOOL_NAME}_qrc})
    add_executable(${TOOL_NAME} WIN32 ${${TOOL_NAME}_src} ${${TOOL_NAME}_header} ${ui_FILES} ${qrc_FILES})
    target_include_directories(${TOOL_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}) # 生成的ui_*.h文件在这里
    target_link_libraries(${TOOL_NAME} PRIVATE ${QT5_LIBS} ${ARGN})
endfunction()

function(build_ut UT_NAME)
    set(${UT_NAME}_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    file(GLOB_RECURSE ${UT_NAME}_header "${${UT_NAME}_DIR}/*.h" "${${UT_NAME}_DIR}/*.hpp")
    file(GLOB_RECURSE ${UT_NAME}_src "${${UT_NAME}_DIR}/*.c" "${${UT_NAME}_DIR}/*.cpp")
    add_executable(${UT_NAME} ${${UT_NAME}_src} ${${UT_NAME}_header})
    target_link_libraries(${UT_NAME} PRIVATE ${SYSTEM_LIBS} ${ARGN})
endfunction()

