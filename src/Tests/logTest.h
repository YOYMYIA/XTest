//
// Created by 76426 on 2025/11/9.
//

#ifndef XTEST_LOGTEST_H
#define XTEST_LOGTEST_H
#include <gtest/gtest.h>
#include "log/log.h"

class LogTest : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if their bodies would
    // be empty.

    LogTest() {
        // You can do set-up work for each test here.
    }

    ~LogTest() override {
        // You can do clean-up work that doesn't throw exceptions here.
    }
    static void SetUpTestSuite() {
        spdlog::info("run before first case");
    }

    static void TearDownTestSuite() {
        spdlog::info("run after last case");
    }
    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override {
        spdlog::info("SetUp()");
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    void TearDown() override {
        spdlog::info("TearDown()");
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

public:
    static std::shared_ptr<class logger> m_log;
};

std::shared_ptr<class logger> LogTest::m_log = std::make_shared<class logger>();

TEST_F(LogTest,init)
{
    m_log->initialize("LOGTEST");
    EXPECT_EQ(0, 0);
}

#endif //XTEST_LOGTEST_H
