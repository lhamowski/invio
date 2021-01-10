#include "connection.hpp"

namespace invio::core::rtsp {

connection::connection(invio::core::tcp_socket& socket) :
    socket_{std::move(socket)}
{
}

void connection::read() {}

}  // namespace invio::core::rtsp