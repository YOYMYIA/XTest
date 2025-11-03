#include <iostream>
#include "Base.h"
#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    std::cout << "hello world" << std::endl;
    std::cout << add(10, 20) << std::endl;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
