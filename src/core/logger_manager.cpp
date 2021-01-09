#include "logger_manager.hpp"
#include "support/assert.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace invio::core {

namespace detail {

spdlog::level::level_enum to_spdlog_level(log_level lvl)
{
    using level = spdlog::level::level_enum;

    switch (lvl)
    {
        case log_level::none:
            return level::off;
        case log_level::trace:
            return level::trace;
        case log_level::debug:
            return level::debug;
        case log_level::info:
            return level::info;
        case log_level::warning:
            return level::warn;
        case log_level::error:
            return level::err;
        case log_level::critical:
            return level::critical;
        default:
            INVIO_ASSERT(false, "Invalid log level");
            return {};
    }
}

}  // namespace detail

logger::logger(std::shared_ptr<spdlog::logger> logger) :
    logger_{std::move(logger)}
{
}

logger_manager::logger_manager(const log_config& cfg)
{
    configure_sinks(cfg);
}

logger_manager::~logger_manager()
{
    spdlog::shutdown();
}

logger& logger_manager::new_logger(const char* name)
{
    logger log_impl{
        std::make_shared<spdlog::logger>(name, sinks_.begin(), sinks_.end())};
    return loggers_.try_emplace(name, std::move(log_impl)).first->second;
}

void logger_manager::configure_sinks(const log_config& cfg)
{
    namespace sinks = spdlog::sinks;

    auto stdout_sink = std::make_shared<sinks::stdout_color_sink_mt>();
    stdout_sink->set_level(detail::to_spdlog_level(cfg.console_level));
    sinks_.push_back(std::move(stdout_sink));

    const auto path = app_data_path() / "invio.log";
    auto file_sink = std::make_shared<sinks::basic_file_sink_mt>(path.string());
    file_sink->set_level(detail::to_spdlog_level(cfg.file_level));
    sinks_.push_back(std::move(file_sink));
}

}  // namespace invio::core