#pragma once

#include <core/net/rtsp/server.hpp>
#include <core/logger_manager.hpp>

#include <kl/reflect_struct.hpp>

#include <filesystem>

namespace invio {

struct config
{
    core::log_config log_cfg{};
    core::rtsp::config rtsp_cfg{};
};
KL_REFLECT_STRUCT(config, log_cfg, rtsp_cfg)

config load_config(const std::filesystem::path& filepath);

}  // namespace invio