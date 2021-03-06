#pragma once

#include <fmt/core.h>

#include <winerror.h>
#include <concepts>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>

namespace invio {

class formatted_exception : public std::exception
{
public:
    template <typename... Args>
    formatted_exception(const char* msg, const Args&... args) :
        msg_{fmt::format(msg, args...)}
    {
    }

    explicit formatted_exception(const char* msg) : msg_{msg} {}
    explicit formatted_exception(const std::string& msg) : msg_{msg} {}

    const char* what() const override { return msg_.c_str(); }

private:
    std::string msg_;
};

class initialization_failed : public formatted_exception
{
public:
    using formatted_exception::formatted_exception;
};

class runtime_error : public formatted_exception
{
public:
    using formatted_exception::formatted_exception;
};

template <typename... Args>
std::string format_error(const std::error_code& error,
                         std::string msg,
                         const Args&... args)
{
    return fmt::format(std::move(msg) + "\n" + error.message(), args...);
}

template <typename Exception, typename... Args>
void throw_if_error(const std::error_code& error,
                    std::string msg,
                    const Args&... args)
{
    if (error)
        throw Exception{format_error(error, std::move(msg), args...)};
}

template <typename Exception, typename... Args>
void throw_if_error(HRESULT error, std::string msg, const Args&... args)
{
    const std::error_code error_code{error, std::system_category()};
    throw_if_error<Exception>(error_code, std::move(msg), args...);
}

}  // namespace invio