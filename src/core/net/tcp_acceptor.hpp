#pragma once

#include "fwd.hpp"

#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace invio::core {

class tcp_acceptor_handler
{
public:
    virtual void on_accepted(invio::core::tcp_socket& socket) = 0;

protected:
    ~tcp_acceptor_handler() = default;
};

class tcp_acceptor final
{
public:
    tcp_acceptor(boost::asio::io_context& ctx, invio::core::logger& logger);

    tcp_acceptor(const tcp_acceptor&) = delete;
    tcp_acceptor& operator=(const tcp_acceptor&) = delete;

    tcp_acceptor(tcp_acceptor&&) = default;
    tcp_acceptor& operator=(tcp_acceptor&&) = delete;

public:
    void start(const boost::asio::ip::tcp::endpoint& endpoint,
               tcp_acceptor_handler& handler);
    void stop();

private:
    class impl;
    std::shared_ptr<impl> impl_;

private:
    boost::asio::io_context& ctx_;
    invio::core::logger& logger_;
};

}  // namespace invio::core