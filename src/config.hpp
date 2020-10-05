#pragma once

#include "core/logger_manager.hpp"

#include <string>

struct config
{
    log_config log_cfg_{};
};

config load_config(const std::string& filepath);