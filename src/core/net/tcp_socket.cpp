#include "tcp_socket.hpp"
#include "support/assert.hpp"

namespace invio::core {

class tcp_socket::impl : public std::enable_shared_from_this<impl>
{
public:
    // already opened socket
    impl(boost::asio::ip::tcp::socket& socket, invio::core::logger& logger) :
        socket_{std::move(socket)}, logger_{logger}
    {
        INVIO_ASSERT(socket_.is_open(), "Socket is closed");
        state_ = state::connected;
    }

    void start() { read(); }

private:
    enum class state
    {
        closed,
        connected
    };

private:
    void read() {}

private:
    boost::asio::ip::tcp::socket socket_;
    state state_{state::closed};

    invio::core::logger& logger_;
};

tcp_socket::tcp_socket(boost::asio::ip::tcp::socket& socket,
                       invio::core::logger& logger) :
    impl_{std::make_shared<impl>(socket, logger)}
{
}

}  // namespace invio::core