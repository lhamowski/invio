#pragma once

#include "support/assert.hpp"
#include "support/path.hpp"

#include <kl/reflect_enum.hpp>
#include <kl/reflect_struct.hpp>

#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace invio {

enum class log_level
{
    none,
    trace,
    debug,
    info,
    warning,
    error,
    critical
};
KL_REFLECT_ENUM(log_level, none, trace, debug, info, warning, error, critical)

struct log_config final
{
    log_level console_level{log_level::info};
    log_level file_level{log_level::info};
};
KL_REFLECT_STRUCT(log_config, console_level, file_level)

namespace detail {

spdlog::level::level_enum to_spdlog_level(log_level lvl);

}

class logger final
{
public:
    explicit logger(std::shared_ptr<spdlog::logger> logger);

    template <typename... Args>
    void log(log_level level, const char* msg, const Args&... args)
    {
        logger_->log(detail::to_spdlog_level(level), msg, args...);
    }

private:
    std::shared_ptr<spdlog::logger> logger_;
};

class logger_manager final
{
public:
    explicit logger_manager(const log_config& cfg);
    ~logger_manager();

    logger new_logger(const char* name);
    const auto& loggers() const { return loggers_; }
    const auto& sinks() const { return sinks_; }

private:
    void configure_sinks(const log_config& cfg);
    std::shared_ptr<spdlog::logger> make_logger(const char* name);

private:
    std::vector<spdlog::sink_ptr> sinks_;
    std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> loggers_;
};

}  // namespace invio

#define LOG(logger, level, ...) (logger).log((level), __VA_ARGS__);
#define LOG_TRACE(log, ...) LOG(log, invio::log_level::trace, __VA_ARGS__)
#define LOG_DEBUG(log, ...) LOG(log, invio::log_level::debug, __VA_ARGS__)
#define LOG_INFO(log, ...) LOG(log, invio::log_level::info, __VA_ARGS__)
#define LOG_WARNING(log, ...) LOG(log, invio::log_level::warning, __VA_ARGS__)
#define LOG_ERROR(log, ...) LOG(log, invio::log_level::error, __VA_ARGS__)
#define LOG_CRITICAL(log, ...) LOG(log, invio::log_level::critical, __VA_ARGS__)