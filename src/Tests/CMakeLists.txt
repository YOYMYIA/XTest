include_directories(
        ${PROJ_SRC_DIR}/LibTest
        ${PROJ_SRC_DIR}/Base/spdlog
        ${PROJ_SRC_DIR}/
        ${OpenCV_LIBS}/
)


build_ut(Tests LibTest GTest::gtest Base XPlat ${OpenCV_LIBS} glfw glad CUDA::cudart)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
copy_directory_files("${OPENCV_DIR}/x64/mingw/bin" ${LIBRARY_OUTPUT_PATH})
endif()