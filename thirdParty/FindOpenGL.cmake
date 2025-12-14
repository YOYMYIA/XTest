
set(GLFW_DIR "${THIRD_PARTY_PREFIX}/glfw")
list(APPEND CMAKE_PREFIX_PATH ${GLFW_DIR})
find_package(glfw REQUIRED)

set(GLAD_DIR "${THIRD_PARTY_PREFIX}/glad")
add_library(glad ${GLAD_DIR}/include/glad/glad.h ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include/)
