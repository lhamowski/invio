#include <core/logger_manager.hpp>
#include <core/net/tcp_acceptor.hpp>
#include <core/net/tcp_socket.hpp>

#include <spdlog/logger.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <catch2/catch.hpp>

#include <cstdint>
#include <functional>
#include <span>

namespace {

class tcp_sock_handler : public invio::core::net::tcp_socket_handler
{
public:
    void received(std::span<const std::byte> data) override 
    {
        REQUIRE(received_data_cb);
        received_data_cb(data);
    }

    void connected() override
    {
        REQUIRE(connected_cb);
        connected_cb();
    }

    void disconnected() override
    {
        REQUIRE(disconnected_cb);
        disconnected_cb();
    }

    std::function<void(std::span<const std::byte>)> received_data_cb;
    std::function<void()> connected_cb;
    std::function<void()> disconnected_cb;
};

}  // namespace

TEST_CASE("tcp socket", "[net]")
{
    using namespace invio::core;

    boost::asio::io_context ctx;
    tcp_sock_handler handler;
    logger log{std::make_shared<spdlog::logger>("test-logger")};

    net::tcp_socket socket{ctx, handler, log};
    net::tcp_socket_properties props{};

    SECTION("connect to localhost")
    {
        boost::asio::ip::tcp::endpoint ep{
            boost::asio::ip::make_address("127.0.0.1"), 12345};
        boost::asio::ip::tcp::acceptor acceptor{ctx, ep};
        boost::asio::ip::tcp::socket raw_socket{ctx};

        std::size_t connection_count{};
        handler.connected_cb = [&] {
            ++connection_count;
            socket.stop(false);
        };
        handler.disconnected_cb = [&] { ctx.stop(); };

        acceptor.async_accept(raw_socket, [](const auto& ec) {});
        socket.connect("localhost", ep.port(), props);
        ctx.run();

        CHECK(connection_count == 1);
    }

    SECTION("connect to unknown host")
    {
        boost::asio::ip::tcp::endpoint ep{
            boost::asio::ip::make_address("127.0.0.1"), 12345};
        boost::asio::ip::tcp::acceptor acceptor{ctx, ep};
        boost::asio::ip::tcp::socket raw_socket{ctx};

        std::size_t connection_count{};
        handler.connected_cb = [&] {
            ++connection_count;
            socket.stop(false);
        };
        handler.disconnected_cb = [&] { ctx.stop(); };

        acceptor.async_accept(raw_socket, [](const auto& ec) {});
        socket.connect("www.test.test.test.test.net", ep.port(), props);
        ctx.run();

        CHECK(connection_count == 0);
    }

    SECTION("read some data")
    {
        boost::asio::ip::tcp::endpoint ep{
            boost::asio::ip::make_address("127.0.0.1"), 12345};
        boost::asio::ip::tcp::acceptor acceptor{ctx, ep};
        boost::asio::ip::tcp::socket raw_socket{ctx};

        std::span<const std::byte> received_data;
        char data[] = {'a', 'b', 'c'};

        handler.connected_cb = [&] {
            socket.send(std::as_bytes(std::span{data}));
        };
        handler.disconnected_cb = [&] { ctx.stop(); };
        handler.received_data_cb = [&](std::span<const std::byte> data) {
            REQUIRE(data.size() == 3);
            CHECK(data[0] == std::byte{'a'});
            CHECK(data[1] == std::byte{'b'});
            CHECK(data[2] == std::byte{'c'});
            socket.stop(false);
        };

        acceptor.async_accept(raw_socket, [&](const auto& ec) {
            REQUIRE_FALSE(ec);
            raw_socket.write_some(boost::asio::buffer(data));
        });
        socket.connect("127.0.0.1", ep.port(), props);

        ctx.run();
    }
}