#pragma once

#include "fwd.hpp"
#include "core/net/tcp_socket.hpp"

#include <boost/asio/ip/tcp.hpp>

#include <span>

namespace invio::core::rtsp {

class connection : public invio::core::net::tcp_socket_handler
{
public:
    connection(boost::asio::ip::tcp::socket&& socket,
               boost::asio::io_context& ctx,
               invio::core::logger_manager& logger_manager);

private:
    // tcp_socket_handler implementation
    void data_received(std::span<const std::byte> data) override;
    void data_sent() override;
    void connected() override;
    void disconnected() override;

private:
    invio::core::net::tcp_socket socket_;
    invio::core::logger& logger_;
};

}  // namespace invio::core::rtsp