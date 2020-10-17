#pragma once

#include <string>

namespace invio {

struct config
{
};

config load_config(const std::string& filepath);

}  // namespace invio