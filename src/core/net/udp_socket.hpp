#pragma once

#include "fwd.hpp"

#include <boost/asio/ip/udp.hpp>

#include <cstddef>
#include <span>

namespace invio::core::net {

class udp_socket_handler
{
public:
    virtual void
        received(std::span<const std::byte> data,
                 const boost::asio::ip::udp::endpoint& remote_endpoint) = 0;
    virtual void sent(bool success,
                      const boost::asio::ip::udp::endpoint& dest) = 0;

protected:
    ~udp_socket_handler() = default;
};

class udp_socket final
{
public:
    udp_socket(boost::asio::io_context& ctx, udp_socket_handler& handler,
               invio::core::logger& logger);

    ~udp_socket();

    udp_socket(const udp_socket&) = delete;
    udp_socket& operator=(const udp_socket&) = delete;

    udp_socket(udp_socket&&) = default;
    udp_socket& operator=(udp_socket&&) = default;

public:
    void start();
    void stop(bool send_pending);
    void bind(const boost::asio::ip::udp::endpoint& ep);
    void send_to(std::span<const std::byte> data,
                 const boost::asio::ip::udp::endpoint& dest);

private:
    boost::asio::io_context& ctx_;
    udp_socket_handler& handler_;
    invio::core::logger& logger_;

private:
    class impl;
    std::shared_ptr<impl> impl_;
};

}  // namespace invio::core::net
