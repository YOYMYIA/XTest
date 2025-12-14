function(add_header_only_library lib_name include_path)
    add_library(${lib_name} INTERFACE)
    target_include_directories(${lib_name} INTERFACE ${include_path})

    # 设置一些通用属性
    target_compile_features(${lib_name} INTERFACE cxx_std_17)
endfunction()

## 注册头文件库
#add_header_only_library(catch2 ${PROJECT_SOURCE_DIR}/third_party/headers/catch2/include)
#add_header_only_library(nlohmann_json ${PROJECT_SOURCE_DIR}/third_party/headers/nlohmann_json/include)
#
## 使用示例
#target_link_libraries(my_test PRIVATE catch2)
#target_link_libraries(my_json_app PRIVATE nlohmann_json)