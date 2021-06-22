#include <core/net/udp_socket.hpp>
#include <core/logger_manager.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <catch2/catch.hpp>

#include <cstdint>
#include <functional>
#include <span>

namespace {

using udp = boost::asio::ip::udp;

class udp_sock_handler : public invio::core::net::udp_socket_handler
{
public:
    // udp_socket_handler implementation
    void data_received(std::span<const std::byte> data,
                       const udp::endpoint& remote_endpoint) override
    {
        REQUIRE(data_received_cb);
        data_received_cb(data, remote_endpoint);
    }

    void data_sent(bool success, const udp::endpoint& dest) override
    {
        REQUIRE(data_sent_cb);
        data_sent_cb(success, dest);
    }

    std::function<void(std::span<const std::byte>, const udp::endpoint&)>
        data_received_cb;
    std::function<void(bool, const udp::endpoint&)> data_sent_cb;
};
}  // namespace

TEST_CASE("udp socket", "[net]") 
{
    using namespace invio::core;

    boost::asio::io_context ctx;
    udp_sock_handler handler;
    logger log{std::make_shared<spdlog::logger>("test-logger")};

    net::udp_socket socket{ctx, handler, log};

    SECTION("send data")
    {
        socket.start();

        char data[3]{'a', 'b', 'c'};
        char received[3]{};

        const boost::asio::ip::udp::endpoint ep{
            boost::asio::ip::make_address("127.0.0.1"), 12345};

        boost::asio::ip::udp::socket raw_socket{ctx, boost::asio::ip::udp::v4()};
        raw_socket.bind(ep);
        socket.send_to(std::as_bytes(std::span{data}), ep);

        handler.data_sent_cb = [&](bool success,
                                   const boost::asio::ip::udp::endpoint& dest) {
            CHECK(success);
            CHECK(dest == ep);
            raw_socket.receive(boost::asio::buffer(received));
            CHECK(received[0] == 'a');
            CHECK(received[1] == 'b');
            CHECK(received[2] == 'c');
            socket.stop(false);
        };

        ctx.run();
    }
}