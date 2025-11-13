//
// Created by 76426 on 2025/11/12.
//

#include "LoggerSystem.h"
#include <filesystem>
#include <iostream>



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
    if (config.auto_create_dir && ((int)config.targets & (int)OutputTarget::FILE))
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
    if ((int)config.targets & (int)OutputTarget::CONSOLE)
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern(config.pattern);
        sinks_.push_back(console_sink);
    }

    // File Output
    if ((int)config.targets & (int)OutputTarget::FILE)
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


bool LoggerSystem::setupAsyncLogger(const LoggerConfig& config)
{
    spdlog::init_thread_pool(config.queue_size, 1);
    default_logger_ = std::make_shared<spdlog::async_logger>(
            config.name,
            sinks_.begin(),
            sinks_.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block
    );
    applyConfigToLogger(config, default_logger_);
    spdlog::register_logger(default_logger_);
    return true;
}

bool LoggerSystem::setupSyncLogger(const LoggerConfig& config)
{
    default_logger_ = std::make_shared<spdlog::logger>(
            config.name, sinks_.begin(), sinks_.end());
    applyConfigToLogger(config, default_logger_);
    spdlog::register_logger(default_logger_);
    return true;
}

void LoggerSystem::applyConfigToLogger(const LoggerConfig& config,
                                       std::shared_ptr<spdlog::logger> logger)
{
    logger->set_level(static_cast<spdlog::level::level_enum>(config.level));

    if (config.flush_on_error)
    {
        logger->flush_on(static_cast<spdlog::level::level_enum>(LogLevel::LOG_ERROR));
    }

    // 设置刷新间隔
    spdlog::flush_every(std::chrono::seconds(config.flush_interval));
}

bool LoggerSystem::registerModule(const std::string& module_name, LogLevel level, bool enabled)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        std::cerr << "Logger system not initialized" << std::endl;
        return false;
    }

    if (modules_.find(module_name) != modules_.end()) {
        LOG_WARN("Module {} already registered", module_name);
        return true;
    }

    modules_.emplace(module_name, LogModule(module_name, level, enabled));

    if (module_name != "default") {
        std::shared_ptr<spdlog::logger> module_logger;

        if (current_config_.async_mode) {
            module_logger = std::make_shared<spdlog::async_logger>(
                    module_name,
                    sinks_.begin(),
                    sinks_.end(),
                    spdlog::thread_pool(),
                    spdlog::async_overflow_policy::block
            );
        } else {
            module_logger = std::make_shared<spdlog::logger>(
                    module_name, sinks_.begin(), sinks_.end());
        }

        module_logger->set_level(static_cast<spdlog::level::level_enum>(level));
        if (current_config_.flush_on_error) {
            module_logger->flush_on(static_cast<spdlog::level::level_enum>(LogLevel::LOG_ERROR));
        }

        spdlog::register_logger(module_logger);
        module_loggers_[module_name] = module_logger;
    }

    LOG_INFO("Module {} registered with level {}", module_name,
             spdlog::level::to_string_view(static_cast<spdlog::level::level_enum>(level)));

    return true;
}


bool LoggerSystem::unregisterModule(const std::string& module_name)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (module_name == "default")
    {
        LOG_ERROR("Cannot unregister default module");
        return false;
    }

    auto mod_it = modules_.find(module_name);
    if (mod_it == modules_.end())
    {
        LOG_WARN("Module {} not found", module_name);
        return false;
    }

    modules_.erase(mod_it);

    auto logger_it = module_loggers_.find(module_name);
    if (logger_it != module_loggers_.end())
    {
        spdlog::drop(module_name);
        module_loggers_.erase(logger_it);
    }

    LOG_INFO("Module {} unregistered", module_name);
    return true;
}

void LoggerSystem::setModuleLevel(const std::string& module_name, LogLevel level)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto mod_it = modules_.find(module_name);
    if (mod_it != modules_.end())
    {
        mod_it->second.level = level;

        auto logger_it = module_loggers_.find(module_name);
        if (logger_it != module_loggers_.end())
        {
            logger_it->second->set_level(static_cast<spdlog::level::level_enum>(level));
        } else if (module_name == "default")
        {
            default_logger_->set_level(static_cast<spdlog::level::level_enum>(level));
        }

        LOG_INFO("Module {} level set to {}", module_name,
                 spdlog::level::to_string_view(static_cast<spdlog::level::level_enum>(level)));
    }
    else
    {
        LOG_WARN("Module {} not found", module_name);
    }
}

void LoggerSystem::enableModule(const std::string& module_name, bool enable)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto mod_it = modules_.find(module_name);
    if (mod_it != modules_.end())
    {
        mod_it->second.enabled = enable;

        auto level = enable ? mod_it->second.level : LogLevel::LOG_OFF;
        auto logger_it = module_loggers_.find(module_name);

        if (logger_it != module_loggers_.end())
        {
            logger_it->second->set_level(static_cast<spdlog::level::level_enum>(level));
        }
        else if (module_name == "default")
        {
            default_logger_->set_level(static_cast<spdlog::level::level_enum>(level));
        }

        LOG_INFO("Module {} {}", module_name, enable ? "enabled" : "disabled");
    } else {
        LOG_WARN("Module {} not found", module_name);
    }
}

std::shared_ptr<spdlog::logger> LoggerSystem::getLogger()
{
    return getLogger("default");
}

std::shared_ptr<spdlog::logger> LoggerSystem::getLogger(const std::string& module_name)
{
    if (!initialized_)
    {
        // 返回一个临时logger避免崩溃
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        return std::make_shared<spdlog::logger>("temp", console_sink);
    }

    if (module_name == "default")
    {
        return default_logger_;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto it = module_loggers_.find(module_name);
    if (it != module_loggers_.end())
    {
        return it->second;
    }

    // 如果模块不存在，返回默认logger
    LOG_WARN("Module {} not found, using default logger", module_name);
    return default_logger_;
}

void LoggerSystem::setGlobalLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(mutex_);

    current_config_.level = level;
    default_logger_->set_level(static_cast<spdlog::level::level_enum>(level));

    // 更新所有模块的级别
    for (auto& [name, module] : modules_)
    {
        if (name != "default")
        {
            module.level = level;
            auto logger_it = module_loggers_.find(name);
            if (logger_it != module_loggers_.end()) {
                logger_it->second->set_level(static_cast<spdlog::level::level_enum>(level));
            }
        }
    }

    LOG_INFO("Global log level set to {}",
             spdlog::level::to_string_view(static_cast<spdlog::level::level_enum>(level)));
}

void LoggerSystem::setFlushLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(mutex_);

    default_logger_->flush_on(static_cast<spdlog::level::level_enum>(level));

    for (auto& [name, logger] : module_loggers_)
    {
        logger->flush_on(static_cast<spdlog::level::level_enum>(level));
    }

    LOG_INFO("Flush level set to {}",
             spdlog::level::to_string_view(static_cast<spdlog::level::level_enum>(level)));
}

void LoggerSystem::flush()
{
    if (default_logger_) {
        default_logger_->flush();
    }

    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [name, logger] : module_loggers_) {
        logger->flush();
    }
}

void LoggerSystem::shutdown()
{
    std::lock_guard<std::mutex> lock(mutex_);

    flush();
    spdlog::shutdown();

    default_logger_.reset();
    module_loggers_.clear();
    modules_.clear();
    sinks_.clear();

    initialized_ = false;

    std::cout << "Logger system shutdown" << std::endl;
}

LoggerSystem::~LoggerSystem()
{
    shutdown();
}

// 状态查询
LogLevel LoggerSystem::getGlobalLevel() const
{
    return current_config_.level;
}

std::vector<std::string> LoggerSystem::getRegisteredModules() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> modules;
    for (const auto& [name, module] : modules_)
    {
        modules.push_back(name);
    }

    return modules;
}

// 配置管理
void LoggerSystem::updateConfig(const LoggerConfig& config)
{
    std::lock_guard<std::mutex> lock(mutex_);

    // 保存当前模块状态
    auto old_modules = modules_;

    // 关闭当前系统
    shutdown();

    // 重新初始化
    if (initialize(config))
    {
        // 重新注册模块
        for (const auto& [name, module] : old_modules)
        {
            if (name != "default")
            {
                registerModule(name, module.level, module.enabled);
            }
        }
    }
}

LoggerConfig LoggerSystem::getCurrentConfig() const
{
    return current_config_;
}

// ==================== LoggerManager ====================

LoggerManager& LoggerManager::instance()
{
    static LoggerManager instance;
    return instance;
}

bool LoggerManager::initSimple(const std::string& log_dir,
                               LogLevel level, bool console, bool file)
{
    LoggerConfig config;
    config.level = level;
    config.targets = OutputTarget::CONSOLE;

    if (file) {
        config.targets = static_cast<OutputTarget>((int)config.targets | (int)OutputTarget::FILE);
        config.file_path = log_dir + "/app.log";
    }

    return LoggerSystem::instance().initialize(config);
}

bool LoggerManager::createModule(const std::string& module_name, LogLevel level)
{
    return LoggerSystem::instance().registerModule(module_name, level);
}

void LoggerManager::setLevel(LogLevel level)
{
    LoggerSystem::instance().setGlobalLevel(level);
}

void LoggerManager::setModuleLevel(const std::string& module_name, LogLevel level)
{
    LoggerSystem::instance().setModuleLevel(module_name, level);
}

void LoggerManager::flush()
{
    LoggerSystem::instance().flush();
}

void LoggerManager::shutdown()
{
    LoggerSystem::instance().shutdown();
}

}