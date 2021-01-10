#pragma once

namespace invio {

namespace core {
class logger;
class logger_manager;
class tcp_acceptor;
class tcp_socket;
}  // namespace core

namespace core::rtsp {
class connection;
class connection_manager;
class server;
}  // namespace core::net::rtsp
}  // namespace invio

namespace boost::asio {
class io_context;
}