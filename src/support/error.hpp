#pragma once

#include <fmt/core.h>

#include <winerror.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>

namespace invio {

struct initialization_failed : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

std::string format_error(const std::error_code& error, std::string error_msg);

template <typename Exception>
void throw_if_error(const std::error_code& error, std::string error_msg)
{
    if (error)
        throw Exception{format_error(error, std::move(error_msg))};
}

template <typename Exception>
void throw_if_error(HRESULT error, std::string error_msg)
{
    const std::error_code error_code{error, std::system_category()};
    throw_if_error<Exception>(error_code, std::move(error_msg));
}

}  // namespace invio