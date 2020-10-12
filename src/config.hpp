#pragma once

#include "core/logger_manager.hpp"

#include <kl/reflect_struct.hpp>

#include <string>

namespace invio {

struct config
{
    log_config log_cfg{};
};
KL_REFLECT_STRUCT(config, log_cfg)

config load_config(const std::string& filepath);

}  // namespace invio