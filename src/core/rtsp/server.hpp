#pragma once

#include "connection_manager.hpp"
#include "fwd.hpp"

#include "core/net/tcp_acceptor.hpp"

#include <kl/reflect_struct.hpp>

#include <boost/asio/ip/tcp.hpp>

#include <cstdint>
#include <memory>

namespace invio::core::rtsp {

struct config final
{
    std::uint16_t port{1995};
};
KL_REFLECT_STRUCT(config, port)

class server final : invio::core::net::tcp_acceptor_handler
{
public:
    server(const config& cfg,
           invio::core::logger& logger,
           boost::asio::io_context& ctx);

    server(const server&) = delete;
    server& operator=(const server&) = delete;

private:
    // invio::core::net::tcp_acceptor_handler implementation
    void on_accepted(invio::core::net::tcp_socket& socket);

private:
    const config& cfg_;
    invio::core::logger& logger_;
    boost::asio::io_context& ctx_;

private:
    connection_manager connection_manager_;
    invio::core::net::tcp_acceptor acceptor_;
};

}  // namespace invio::core::rtsp