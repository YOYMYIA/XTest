include_directories(
        ${PROJ_SRC_DIR}/LibTest
        ${PROJ_SRC_DIR}/Base/spdlog
        ${PROJ_SRC_DIR}/
)

build_ut(Tests LibTest GTest::gtest Base XPlat)