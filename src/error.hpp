#pragma once

#include <stdexcept>
#include <string>

#include "fmt/format.h"

namespace invio {

struct initialization_failed : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

template <typename Exception>
void handle_error(const std::error_code& error, std::string error_msg)
{
    if (error)
    {
        throw Exception{
            fmt::format(std::move(error_msg) + "\n{}", error.message())};
    }
}

template <typename Exception>
void handle_error(HRESULT error, std::string error_msg)
{
    const std::error_code error_code{error, std::system_category()};
    handle_error<Exception>(error_code, error_msg);
}

}  // namespace invio