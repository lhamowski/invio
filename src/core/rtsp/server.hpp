#pragma once

#include "fwd.hpp"

#include <kl/reflect_struct.hpp>

#include <boost/asio/ip/tcp.hpp>

#include <cstdint>
#include <memory>

namespace boost::asio {
class io_context;
}

namespace invio::core::rtsp {

struct config final
{
    std::uint16_t port{1995};
};
KL_REFLECT_STRUCT(config, port)

class server final
{
public:
    server(const config& cfg,
           invio::core::logger& logger,
           boost::asio::io_context& ctx);

    server(const server&) = delete;
    server& operator=(const server&) = delete;

private:
    const config& cfg_;
    invio::core::logger& logger_;

    boost::asio::io_context& ctx_;
    boost::asio::ip::tcp::socket socket_;

private:
    class listener;
    std::shared_ptr<listener> listener_;
};

}  // namespace invio::core::rtsp