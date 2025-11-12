//
// Created by 76426 on 2025/11/10.
//

#ifndef XTEST_LOGGERDEFINES_H
#define XTEST_LOGGERDEFINES_H

#include <string>

namespace LoggerSystem
{
    //Define Log Lever
    enum class LogLevel : int8_t
    {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        CRITICAL = 5,
        OFF = 6
    };

    //Log Output Target
    enum class OutputTarget
    {
        CONSOLE = 0x01,
        FILE = 0x02,
        NETWORK = 0x04,
        ALL = CONSOLE | FILE | NETWORK
    };

    enum class FileRollingPolicy
    {
        NONE,           //  NO Rolling
        DAILY,
        HOURLY,
        SIZE_BASED,
        SIZE_AND_TIME
    };

    // The module of log define
    struct LogModule {
        std::string name;
        LogLevel level;
        bool enabled;

        LogModule(const std::string& n, LogLevel l = LogLevel::INFO, bool e = true)
                : name(n), level(l), enabled(e) {}
    };


    struct LoggerConfig
    {
        // Base Config
        std::string name{"default"};
        LogLevel level{LogLevel::INFO};
        OutputTarget targets{OutputTarget::CONSOLE};

        // Form
        std::string pattern{"[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%n] [%s:%#] %v"};
        bool enable_color{true};

        // File Config
        std::string file_path{"logs/app.log"};
        FileRollingPolicy file_policy{FileRollingPolicy::SIZE_BASED};
        size_t max_file_size{100 * 1024 * 1024}; // 100MB
        size_t max_files{10};
        std::string file_extension{".log"};

        bool async_mode{true};
        size_t queue_size{8192};
        uint32_t flush_interval{3}; // seconds

        // 高级配置
        bool flush_on_error{true};
        bool auto_create_dir{true};
        bool truncate_file{false};
    };

    struct NetworkConfig
    {
        std::string host;
        uint16_t port;
        std::string endpoint;
        int timeout_sec{5};
    };
}


#endif //XTEST_LOGGERDEFINES_H
