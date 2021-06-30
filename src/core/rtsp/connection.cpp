#include "core/rtsp/connection.hpp"
#include "core/logger_manager.hpp"

#include <utility>

namespace invio::core::rtsp {

connection::connection(boost::asio::ip::tcp::socket&& socket,
                       boost::asio::io_context& ctx,
                       invio::core::logger_manager& logger_manager) :
    socket_{ctx, *this, logger_manager.new_logger("tcp-socket")},
    logger_{logger_manager.new_logger("rtsp-connection")}
{
    socket_.connect(std::move(socket));
}

void connection::data_received(std::span<const std::byte> data)
{
}

void connection::data_sent()
{
}

void connection::connected()
{
}

void connection::disconnected()
{
}

}