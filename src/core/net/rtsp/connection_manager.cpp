#include "connection_manager.hpp"
#include "connection.hpp"
#include "core/logger_manager.hpp"

namespace invio::core::rtsp {

connection_manager::connection_manager(invio::core::logger& logger) :
    logger_{logger}
{
}

void connection_manager::new_connection(invio::core::tcp_socket& socket)
{
    LOG_INFO(logger_, "New connection");
    connections_.insert(std::make_shared<connection>(socket));
}

}  // namespace invio::core::rtsp