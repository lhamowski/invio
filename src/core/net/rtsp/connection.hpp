#pragma once

#include "core/net/tcp_socket.hpp"

#include <memory>

namespace invio::core::rtsp {

class connection final : public std::enable_shared_from_this<connection>
{
public:
    connection(invio::core::tcp_socket& socket);

    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

private:
    void read();

private:
    invio::core::tcp_socket socket_;
};

}  // namespace invio::core::rtsp