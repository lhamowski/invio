#pragma once

#include <core/logger_manager.hpp>

#include <kl/reflect_struct.hpp>

#include <filesystem>

namespace invio {

struct config
{
    core::log_config log_cfg{};
};
KL_REFLECT_STRUCT(config, log_cfg)

config load_config(const std::filesystem::path& filepath);

}  // namespace invio