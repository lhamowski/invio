#pragma once

#include "fwd.hpp"

#include <boost/asio/ip/tcp.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

namespace invio::core::net {

struct tcp_socket_properties
{
    bool keepalive{false};
    bool reconnect{true};
    std::size_t max_reconnects{5};
    std::chrono::seconds reconnect_interval{5};
};

class tcp_socket_handler
{
public:
    virtual void received(std::span<const std::byte> data) = 0;
    virtual void connected() = 0;
    virtual void disconnected() = 0;

protected:
    ~tcp_socket_handler() = default;
};

class tcp_socket final
{
public:
    tcp_socket(boost::asio::io_context& ctx, tcp_socket_handler& handler,
               invio::core::logger& logger);

    ~tcp_socket();

    tcp_socket(const tcp_socket&) = delete;
    tcp_socket& operator=(const tcp_socket&) = delete;

    tcp_socket(tcp_socket&&) = default;
    tcp_socket& operator=(tcp_socket&&) = default;

public:
    void connect(const char* host, std::uint16_t port,
                 const tcp_socket_properties& props);
    void connect(boost::asio::ip::tcp::socket&& socket,
                 const tcp_socket_properties& props);
    void stop(bool send_pending);

    void send(std::span<const std::byte> data);

private:
    boost::asio::io_context& ctx_;
    tcp_socket_handler& handler_;
    invio::core::logger& logger_;

private:
    class impl;
    std::shared_ptr<impl> impl_;
};

}  // namespace invio::core::net
