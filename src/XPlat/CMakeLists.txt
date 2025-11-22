
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

build_shared_library_with_test(XPlat)

# setting the include directories, ensure can find System/Standard.h etc. header files
target_include_directories(XPlat
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)       