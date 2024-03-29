#include "core/rtsp/server.hpp"

#include "core/logger_manager.hpp"
#include "support/error.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/system/error_code.hpp>

#include <string>

namespace invio::core::rtsp {

server::server(const config& cfg, invio::core::logger& logger,
               boost::asio::io_context& ctx) :
    cfg_(cfg),
    logger_{logger},
    ctx_{ctx},
    acceptor_{ctx, logger}
{
    LOG_INFO(logger_, "Starting RTSP Server...");

    boost::asio::ip::tcp::endpoint endpoint{boost::asio::ip::address_v4::any(),
                                            cfg_.port};
    acceptor_.start(endpoint, *this);
}

void server::on_accepted(boost::asio::ip::tcp::socket& socket)
{
    // TODO
}

}  // namespace invio::core::rtsp
