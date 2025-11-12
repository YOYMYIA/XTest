//
// Created by 76426 on 2025/11/12.
//

#ifndef XTEST_LOGGERSYSTEM_H
#define XTEST_LOGGERSYSTEM_H
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/hourly_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/async.h>

#include "LoggerDefines.h"

class LoggerManager;
namespace LoggerSystem {

class LoggerSystem {
public:
    static LoggerSystem &instance();

    // Disable copy and move
    LoggerSystem(const LoggerSystem &) = delete;

    LoggerSystem &operator=(const LoggerSystem &) = delete;

    // initialize log system
    bool initialize(const LoggerConfig &config = LoggerConfig());
    bool initialize(const std::string &config_file); // Load configuration from file

    //register modules
    bool registerModule(const std::string& module_name, LogLevel level = LogLevel::INFO, bool enabled = true);
    bool unregisterModule(const std::string& module_name);
    void setModuleLevel(const std::string& module_name, LogLevel level);
    void enableModule(const std::string& module_name, bool enable = true);

    //get logger
    std::shared_ptr<spdlog::logger> getLogger();
    std::shared_ptr<spdlog::logger> getLogger(const std::string& module_name);

    //global setting
    void setGlobalLevel(LogLevel level);
    void setFlushLevel(LogLevel level);
    void flush();
    void shutdown();

    //query state
    bool isInitialized() const { return initialized_; }
    LogLevel getGlobalLevel() const;
    std::vector<std::string> getRegisteredModules() const;

    //update manage
    void updateConfig(const LoggerConfig& config);
    LoggerConfig getCurrentConfig() const;

private:
    LoggerSystem() = default;
    ~LoggerSystem();

    bool createSinks(const LoggerConfig& config);
    bool setupAsyncLogger(const LoggerConfig& config);
    bool setupSyncLogger(const LoggerConfig& config);
    void applyConfigToLogger(const LoggerConfig& config, std::shared_ptr<spdlog::logger> logger);

private:
    friend class LoggerManager;

    std::atomic<bool> initialized_{false};
    std::shared_ptr<spdlog::logger> default_logger_;
    std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> module_loggers_;
    std::unordered_map<std::string, LogModule> modules_;
    LoggerConfig current_config_;
    std::vector<spdlog::sink_ptr> sinks_;
    mutable std::mutex mutex_;
};

class LoggerManager {
public:
    static LoggerManager& instance();

    static bool initSimple(const std::string& log_dir = "logs",
                           LogLevel level = LogLevel::INFO,
                           bool console = true, bool file = true);

    static bool createModule(const std::string& module_name,
                             LogLevel level = LogLevel::INFO);

    static void setLevel(LogLevel level);
    static void setModuleLevel(const std::string& module_name, LogLevel level);

    static void flush();
    static void shutdown();
};

// ==================== LOG MACRO ====================

// BASIC
#define LOG_TRACE(...)      SPDLOG_LOGGER_TRACE(LoggerSystem::LoggerSystem::instance().getLogger().get(), __VA_ARGS__)
#define LOG_DEBUG(...)      SPDLOG_LOGGER_DEBUG(LoggerSystem::LoggerSystem::instance().getLogger().get(), __VA_ARGS__)
#define LOG_INFO(...)       SPDLOG_LOGGER_INFO(LoggerSystem::LoggerSystem::instance().getLogger().get(), __VA_ARGS__)
#define LOG_WARN(...)       SPDLOG_LOGGER_WARN(LoggerSystem::LoggerSystem::instance().getLogger().get(), __VA_ARGS__)
#define LOG_ERROR(...)      SPDLOG_LOGGER_ERROR(LoggerSystem::LoggerSystem::instance().getLogger().get(), __VA_ARGS__)
#define LOG_CRITICAL(...)   SPDLOG_LOGGER_CRITICAL(LoggerSystem::LoggerSystem::instance().getLogger().get(), __VA_ARGS__)

// MODEL
#define LOG_MODULE_TRACE(module, ...)    SPDLOG_LOGGER_TRACE(LoggerSystem::LoggerSystem::instance().getLogger(module).get(), __VA_ARGS__)
#define LOG_MODULE_DEBUG(module, ...)    SPDLOG_LOGGER_DEBUG(LoggerSystem::LoggerSystem::instance().getLogger(module).get(), __VA_ARGS__)
#define LOG_MODULE_INFO(module, ...)     SPDLOG_LOGGER_INFO(LoggerSystem::LoggerSystem::instance().getLogger(module).get(), __VA_ARGS__)
#define LOG_MODULE_WARN(module, ...)     SPDLOG_LOGGER_WARN(LoggerSystem::LoggerSystem::instance().getLogger(module).get(), __VA_ARGS__)
#define LOG_MODULE_ERROR(module, ...)    SPDLOG_LOGGER_ERROR(LoggerSystem::LoggerSystem::instance().getLogger(module).get(), __VA_ARGS__)
#define LOG_MODULE_CRITICAL(module, ...) SPDLOG_LOGGER_CRITICAL(LoggerSystem::LoggerSystem::instance().getLogger(module).get(), __VA_ARGS__)

//Condition Log
#define LOG_IF(condition, level, ...) \
    do { \
        if (condition) { \
            SPDLOG_LOGGER_CALL(LoggerSystem::LoggerSystem::instance().getLogger().get(), \
                              spdlog::level::level_enum(static_cast<int>(level)), __VA_ARGS__); \
        } \
    } while(0)

// Once Log
#define LOG_ONCE(level, ...) \
    do { \
        static bool logged = false; \
        if (!logged) { \
            logged = true; \
            SPDLOG_LOGGER_CALL(LoggerSystem::LoggerSystem::instance().getLogger().get(), \
                              spdlog::level::level_enum(static_cast<int>(level)), __VA_ARGS__); \
        } \
    } while(0)

}
#endif //XTEST_LOGGERSYSTEM_H
