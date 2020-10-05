#include "error.hpp"

#include <fmt/core.h>

namespace invio {

std::string format_error(const std::error_code& error, std::string error_msg)
{
    return fmt::format(std::move(error_msg) + "\n{}", error.message());
}

}  // namespace invio