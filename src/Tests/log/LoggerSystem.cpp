//
// Created by 76426 on 2025/11/12.
//

#include "LoggerSystem.h"
#include <filesystem>
#include <fstream>
#include <iostream>

#ifdef PLAT_LINUX
#include <sys/stat.h>
#else
#include <direct.h>
#endif

namespace LoggerSystem {

LoggerSystem &LoggerSystem::instance()
{
    static LoggerSystem instance;
    return instance;
}

bool LoggerSystem::initialize(const LoggerConfig& config)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_)
    {
        LOG_WARN("Logger system already initialized");
        return true;
    }
    current_config_ = config;

    // Create Log Directory
    if (config.auto_create_dir && (config.targets & OutputTarget::FILE))
    {
        std::filesystem::path path(config.file_path);
        auto dir = path.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir))
        {
            std::filesystem::create_directories(dir);
        }
    }

    // 创建sinks
    if (!createSinks(config))
    {
        std::cerr << "Failed to create log sinks" << std::endl;
        return false;
    }

    // 创建logger
    if (config.async_mode) {
        if (!setupAsyncLogger(config))
        {
            return false;
        }
    }
    else
    {
        if (!setupSyncLogger(config))
        {
            return false;
        }
    }

    // 设置默认模块
    registerModule(config.name, config.level, true);

    initialized_ = true;

    // 使用spdlog原生接口记录初始化成功信息，避免递归
    default_logger_->info("Logger system initialized successfully");
    default_logger_->info("Global log level: {}", spdlog::level::to_string_view(
                                  static_cast<spdlog::level::level_enum>(config.level)));

    return true;
}

bool LoggerSystem::initialize(const std::string& config_file)
{
    // Simplify Implementation, Using JSON Config File
    LoggerConfig config;
    config.file_path = config_file;
    config.async_mode = true;

    return initialize(config);
}

bool LoggerSystem::createSinks(const LoggerConfig& config)
{
    sinks_.clear();

    // Console Output
    if (config.targets & OutputTarget::CONSOLE)
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern(config.pattern);
        sinks_.push_back(console_sink);
    }

    // File Output
    if (config.targets & OutputTarget::FILE)
    {
        spdlog::sink_ptr file_sink;

        switch (config.file_policy)
        {
            case FileRollingPolicy::DAILY:
            {
                file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
                        config.file_path, 0, 0); // 每天0点0分滚动
                break;
            }
            case FileRollingPolicy::HOURLY:
            {
                file_sink = std::make_shared<spdlog::sinks::hourly_file_sink_mt>(
                        config.file_path, config.truncate_file);
                break;
            }
            case FileRollingPolicy::SIZE_BASED:
            {
                file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                        config.file_path, config.max_file_size, config.max_files);
                break;
            }
            case FileRollingPolicy::SIZE_AND_TIME:
            {
                //TODO::
                break;
            }
            default:
            {
                //TODO::
                break;
            }
        }

        file_sink->set_pattern(config.pattern);
        sinks_.push_back(file_sink);
    }

    //TODO::NETWORK

    return !sinks_.empty();
}

}