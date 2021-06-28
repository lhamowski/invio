#include <core/net/tcp_acceptor.hpp>
#include <core/net/tcp_socket.hpp>
#include <core/logger_manager.hpp>

#include <spdlog/spdlog.h>

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <catch2/catch.hpp>

#include <cstdint>
#include <functional>

namespace {

class tcp_accept_handler : public invio::core::net::tcp_acceptor_handler
{
public:
    // invio::core::net::tcp_acceptor_handler
    void on_accepted(boost::asio::ip::tcp::socket& socket) override 
    {
        REQUIRE(on_accepted_cb);
        on_accepted_cb(socket);
    }

    std::function<void(boost::asio::ip::tcp::socket&)> on_accepted_cb;
};

}  // namespace

TEST_CASE("tcp acceptor", "[net]")
{
    using namespace invio::core;

    boost::asio::io_context ctx;
    boost::asio::ip::tcp::socket raw_socket{ctx, boost::asio::ip::tcp::v4()};
    tcp_accept_handler handler;
    logger log{std::make_shared<spdlog::logger>("test-logger")};

    net::tcp_acceptor acceptor{ctx, log};

    handler.on_accepted_cb = [&](boost::asio::ip::tcp::socket& socket) {
        REQUIRE(socket.is_open());
        CHECK(socket.remote_endpoint().address().to_string() == "127.0.0.1");

        acceptor.stop();
        raw_socket.close();
        socket.close();
    };

    const boost::asio::ip::tcp::endpoint ep{
        boost::asio::ip::make_address("127.0.0.1"), 12345};
    acceptor.start(ep, handler);

    raw_socket.connect(ep);
    ctx.run();
}