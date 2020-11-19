#pragma once

#include "config.hpp"
#include "core/logger_manager.hpp"

#include <boost/asio/io_context.hpp>

namespace invio {

class app final
{
public:
    app(config& cfg);

public:
    void main_loop();

private:
    config& cfg_;

    boost::asio::io_context ctx_{};

    core::logger_manager logger_manager_;
    core::logger logger_;
};

}  // namespace invio