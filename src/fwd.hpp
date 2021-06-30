#pragma once

namespace invio {

namespace core {
class logger;
class logger_manager;
}  // namespace core

namespace core::net {
class tcp_acceptor;
class tcp_socket;
class udp_socket;
}  // namespace core::net

namespace core::rtsp {
class connection;
class server;
}  // namespace core::rtsp
}  // namespace invio

namespace boost::asio {
class io_context;
}