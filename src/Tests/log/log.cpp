//
// Created by 76426 on 2025/11/8.
//

#include "log.h"
logger::logger()
{

}

void logger::initialize(std::string name)
{
    spdlog::info("Create a logger, the log's name is {}",name.c_str());
    m_log = std::make_shared<spdlog::logger>(name);
    spdlog::initialize_logger(m_log);
}