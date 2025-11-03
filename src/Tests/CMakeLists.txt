include_directories(
        ${PROJ_SRC_DIR}/LibTest/
)

build_ut(Tests LibTest GTest::gtest)