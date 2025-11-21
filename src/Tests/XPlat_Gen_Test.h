#ifndef XPLAT_GEN_TEST_H
#define XPLAT_GEN_TEST_H

#include <gtest/gtest.h>
#include "XPlat/Gen/Core.h"

class XPlatGenTest : public ::testing::Test 
{
public:
    XPlatGenTest() {
        // You can do set-up work for each test here.
    }

    ~XPlatGenTest() override {
        // You can do clean-up work that doesn't throw exceptions here.
    }
};

TEST_F(XPlatGenTest, TestIsCompatibleSignature) 
{
    bool t1 = xplat::gen::IsCompatibleSignature<int(int), int>::value;
    bool t2 = xplat::gen::IsCompatibleSignature<int(float), int>::value;
    EXPECT_TRUE(t1);
    EXPECT_FALSE(t2);
}

#endif // XPLAT_GEN_TEST_H