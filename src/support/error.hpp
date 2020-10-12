#pragma once

#include <fmt/core.h>

#include <winerror.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>

namespace invio {

class initialization_failed : public std::exception
{
public:
    template <typename... Args>
    initialization_failed(const char* msg, const Args&... args) :
        msg_{fmt::format(msg, args...)}
    {
    }

    explicit initialization_failed(const char* msg) : msg_{msg} {}
    explicit initialization_failed(const std::string& msg) : msg_{msg} {}

    const char* what() const override { return msg_.c_str(); }

private:
    std::string msg_;
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