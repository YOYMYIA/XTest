project(GUI)

# 设置Qt5自动功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt5包
find_package(Qt5 REQUIRED COMPONENTS
        Core
        Widgets
        Charts
        PrintSupport
        Gui
        OpenGL
        Xml
        Network
)

# 检查是否找到所有必需的组件
if(NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 not found! Please set Qt5_DIR to the Qt5 installation directory")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SRC_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/"
)


set(PROJECT_SOURCES)
foreach(dir ${SRC_DIRS})
    file(GLOB_RECURSE dir_sources
            "${dir}/*.cpp"
            "${dir}/*.cxx"
            "${dir}/*.cc"
    )
    list(APPEND PROJECT_SOURCES ${dir_sources})
endforeach()

# 收集头文件（可选，用于IDE显示）
set(PROJECT_HEADERS)
foreach(dir ${SRC_DIRS})
    file(GLOB_RECURSE dir_headers
            "${dir}/*.h"
            "${dir}/*.hpp"
            "${dir}/*.hxx"
    )
    list(APPEND PROJECT_HEADERS ${dir_headers})
endforeach()

# 收集UI文件
set(PROJECT_UIS)
foreach(dir ${SRC_DIRS})
    file(GLOB_RECURSE dir_uis
            "${dir}/*.ui"
    )
    list(APPEND PROJECT_UIS ${dir_uis})
endforeach()

# 添加Qt资源文件
qt5_add_resources(QRC_FILES
        #"Res/res.qrc"
        #"UIstyle/qss.qrc"
)

set(EXECUTABLE_OUTPUT_PATH ${PROJ_BIN_DIR})


# Windows特定配置
if(WIN32)
    # Windows资源文件
    set(WIN_RESOURCES
            ../Tests/log/log.cpp
            ../Tests/log/log.h
            ../Tests/logTest.h
            ../Tests/log/LoggerDefines.h
            ../Tests/log/LoggerSystem.cpp
            ../Tests/log/LoggerSystem.h
            ../Tests/Tools/Singleton.h
            #"Res/icon.rc"
    )

    # 根据架构设置目标
    if(PROJ_ARCH MATCHES "x64")
        add_executable(${PROJECT_NAME} WIN32
                ${PROJECT_SOURCES}
                ${PROJECT_HEADERS}
                ${PROJECT_UIS}
                ${QRC_FILES}
                ${WIN_RESOURCES}
        )
    else()
        add_executable(${PROJECT_NAME}
                ${PROJECT_SOURCES}
                ${PROJECT_HEADERS}
                ${PROJECT_UIS}
                ${QRC_FILES}
                ${WIN_RESOURCES}
        )
    endif()
else()
    # Linux/macOS配置
    add_executable(${PROJECT_NAME}
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
            ${PROJECT_UIS}
            ${QRC_FILES}
    )
endif()

# 链接Qt5库
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::Charts
        Qt5::PrintSupport
        Qt5::Gui
        Qt5::OpenGL
        Qt5::Xml
        Qt5::Network
)

# 链接系统库
if(SYSTEM_LIBS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SYSTEM_LIBS})
endif()

# 添加包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        # 添加其他包含目录
)