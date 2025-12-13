#include <gtest/gtest.h>

// #include "logTest.h"
#include "XPlat_Gen_Test.h"
#include "CVtest.h"

int main(int argc, char *argv[])
{
    try {
        test_opencv_core();
        return 0;
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV异常: " << e.what() << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "标准异常: " << e.what() << std::endl;
        return -1;
    }

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
