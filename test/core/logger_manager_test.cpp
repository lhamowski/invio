#include <core/logger_manager.hpp>

#include <catch2/catch.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

TEST_CASE("logger manager", "[logger]")
{
    using namespace invio::core;

    log_config cfg{};
    cfg.console_level = log_level::critical;
    cfg.file_level = log_level::trace;

    logger_manager logger_manager_{cfg};

    const auto& sinks = logger_manager_.sinks();
    const auto& loggers = logger_manager_.loggers();

    SECTION("sink levels")
    {
        REQUIRE(sinks.size() == 2);
        CHECK(sinks[0]->level() == spdlog::level::level_enum::critical);
        CHECK(sinks[1]->level() == spdlog::level::level_enum::trace);
    }

    SECTION("new logger")
    {
        CHECK(loggers.empty());
        auto logger1 = logger_manager_.new_logger("logger1");
        auto logger2 = logger_manager_.new_logger("logger2");
        CHECK(loggers.size() == 2);
        auto logger1_2 = logger_manager_.new_logger("logger1");
        CHECK(loggers.size() == 2);
    }
}