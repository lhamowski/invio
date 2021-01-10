#pragma once

#include "core/logger_manager.hpp"

#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace invio::core {

class tcp_socket final
{
public:
    tcp_socket(boost::asio::ip::tcp::socket& socket,
               invio::core::logger& logger);

private:
    class impl;
    std::shared_ptr<impl> impl_;
};

}  // namespace invio::core