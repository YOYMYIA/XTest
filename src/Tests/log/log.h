//
// Created by 76426 on 2025/11/8.
//

#ifndef XTEST_LOG_H
#define XTEST_LOG_H

#include "spdlog.h"

class logger {
public:
    logger();

    void initialize(std::string name = "");

private:
    std::shared_ptr<spdlog::logger> m_log;

};


#endif //XTEST_LOG_H
