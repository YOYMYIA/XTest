if(PROJ_ARCH MATCHES "x64" OR PROJ_ARCH MATCHES "win64")
    # Windows 平台
    message(STATUS "Configuring for Windows x64")

    set(SYSTEM_LIBS
            setupapi
            ws2_32
            mswsock
            advapi32
            user32
    )

    #set(ZMQ_LIBS, zmq)

    # CURL 库配置
#    if(CMAKE_BUILD_TYPE MATCHES "Debug")
#        set(CURL_LIB_NAME libcurl-d)
#        set(ZLIB_LIB_NAME zlibd)
#    else()
#        set(CURL_LIB_NAME libcurl)
#        set(ZLIB_LIB_NAME zlib)
#    endif()

#    set(CURL_LIBS
#            ${PROJ_TRD_DIR}/${PROJ_ARCH}/lib/${CURL_LIB_NAME}.lib
#            ${PROJ_TRD_DIR}/${PROJ_ARCH}/lib/${ZLIB_LIB_NAME}.lib
#    )

else ()
    # Linux/Unix 平台
    message(STATUS "Configuring for Unix/Linux")

    set(SYSTEM_LIBS
            pthread
            stdc++
            m
            dl
            rt
    )

#    set(ZMQ_LIBS zmq)
#    set(CURL_LIBS
#            ${PROJ_TRD_DIR}/${PROJ_ARCH}/lib/libcurl.a
#            z
#    )
#    set(SSL_LIBS ssl crypto)
#    set(SQLITE_LIBS ${PROJ_TRD_DIR}/${PROJ_ARCH}/lib/libsqlite3.a)
endif()

# 添加库文件存在性检查
function(check_libraries_exist LIB_LIST)
    foreach(LIB ${${LIB_LIST}})
        if(EXISTS ${LIB} OR NOT LIB MATCHES ".*/.*")  # 如果是完整路径则检查存在性
            message(STATUS "Library ${LIB} - found")
        else()
            message(WARNING "Library ${LIB} - not found")
        endif()
    endforeach()
endfunction()

# 检查关键库是否存在
check_libraries_exist(CURL_LIBS)
check_libraries_exist(SQLITE_LIBS)

set(GTEST_LIBS gtest gmock)
set(THIDR_PARTY_PREFIX, "${PROJ_TRD_DIR}/${PROJ_ARCH}")

#set(Eigen3_DIR, "${PROJ_TRD_DIR}/Eigen/...Cmake")
#find_package(Eigen3 REQUIRED)
#include_directories(/include/)

#seat(gFlag_DIR, "/gflag")
#include_directories(/include)
#file(GLOB GFLAGS_LINK_LIB, ".../lib/*")

# 配置选项，方便后续启用
option(ENABLE_QT "Enable Qt support" OFF)
option(ENABLE_GTEST "Enable Google Test" OFF)
option(ENABLE_GFLAGS "Enable gflags library" OFF)
option(ENABLE_GLOG "Enable glog library" OFF)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "Windows platform detected")
    # Windows 特定变量
    set(PLATFORM_DEPENDENT_LIBS "")

    #set(QT5_DIR, $ENV{QT5_DIR})
    # 条件性地包含第三方库
    if(ENABLE_QT)
        message(STATUS "Qt support enabled - remember to set QT5_DIR")
        # set(QT5_DIR $ENV{QT5_DIR})
        # find_package(Qt5 COMPONENTS Core REQUIRED)
    endif()

    #set(GTEST_DIR, "PATH//")
    #find_package(GTest, REQUIRED)
    #file(GLOB gFlags_BINARY "flag/bin/*")
    #file(GLOB glog_BINARY "glog/bin/*")
    #file(COPY ${gflags_BINARY} DESTINATION "${EXECUTABLE_OUTPUT_PATH}/")
    #...
    if(ENABLE_GTEST)
        message(STATUS "GTest enabled - remember to set GTEST_DIR")
        # set(GTEST_DIR "C:/path/to/gtest")
        # find_package(GTest REQUIRED)
    endif()

elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Linux platform detected")
    # Linux 特定变量
    set(PLATFORM_DEPENDENT_LIBS pthread dl)

    #set(QT5_DIR "home/..../Qt5.14.2/gcc_64/lib/cmake/")
    #set(GTest_DIR ${$THRD_PARTY_PREFIX}/googletest})
    #include_directories(${GTest_DIR}/include)
    #file(GLOB gFlags_BINARY ${gFlags_DIR}/lib/*)
    #...
    if(ENABLE_QT)
        message(STATUS "Qt support enabled - remember to set QT5_DIR")
        # set(QT5_DIR "/opt/qt/5.14.2/gcc_64/lib/cmake/Qt5")
        # find_package(Qt5 COMPONENTS Core REQUIRED)
    endif()

    if(ENABLE_GTEST)
        message(STATUS "GTest enabled - remember to set GTest_DIR")
        # set(GTest_DIR "/usr/local/googletest")
        # include_directories(${GTest_DIR}/include)
    endif ()
else()
    message(STATUS "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif ()

# 通用的库复制函数（待启用）
function(copy_third_party_binaries)
    if(ENABLE_GFLAGS OR ENABLE_GLOG)
        message(STATUS "Binary copy functionality commented out - enable after library installation")
        # if(PLATFORM_WINDOWS)
        #   # Windows DLL 复制逻辑
        # elseif(PLATFORM_LINUX)
        #   # Linux SO 复制逻辑
        # endif()
    endif()
endfunction()


#if(WITH_QT)
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

    # 定义QT5_LIBS变量，包含所有需要链接的Qt5模块
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

    # 添加Qt5宏定义和自动处理MOC
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    message(STATUS "Qt5 support enabled")
#endif()
