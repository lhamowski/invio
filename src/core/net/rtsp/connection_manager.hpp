#pragma once

#include "fwd.hpp"

#include <memory>
#include <unordered_set>

namespace invio::core::rtsp {

class connection_manager final
{
public:
    connection_manager(invio::core::logger& logger);

    void new_connection(invio::core::tcp_socket& socket);

private:
    invio::core::logger& logger_;
    std::unordered_set<std::shared_ptr<connection>> connections_;
};

}  // namespace invio::core::rtsp