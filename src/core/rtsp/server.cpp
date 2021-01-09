#include "server.hpp"
#include "core/logger_manager.hpp"
#include "support/error.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>

#include <boost/system/error_code.hpp>

#include <string>

namespace invio::core::rtsp {

class server::listener : public std::enable_shared_from_this<listener>
{
public:
    listener(invio::core::logger& logger,
             boost::asio::io_context& ctx,
             const boost::asio::ip::tcp::endpoint& endpoint) :
        logger_{logger}, acceptor_{ctx}
    {
        listen(endpoint);
    }

    ~listener() { acceptor_.close(); }

private:
    void listen(const boost::asio::ip::tcp::endpoint& endpoint)
    {
        try
        {
            acceptor_.open(endpoint.protocol());
            acceptor_.bind(endpoint);
            acceptor_.listen();

            LOG_INFO(logger_,
                "Started listening for incoming connections (port={})",
                endpoint.port());
        }
        catch (const std::exception& ex)
        {
            LOG_ERR(logger_,
                "Cannot start listening for incoming connections (port={})",
                    endpoint.port());
            throw invio::initialization_failed(ex.what());
        }
    }

    void accept()
    {
        acceptor_.async_accept(
            [self = shared_from_this()](const boost::system::error_code& ec,
                                        boost::asio::ip::tcp::socket socket) {
                self->on_accepted(ec, socket);
            });
    }

    void on_accepted(const boost::system::error_code& ec,
                     boost::asio::ip::tcp::socket& socket)
    {
        if (ec)
        {
            LOG_WARN(logger_, "Cannot accept connection (error={})",
                     ec.message());
        }
        else
        {
            LOG_INFO(logger_, "New connection");
            // TODO connection manager
        }
    }

private:
    invio::core::logger& logger_;
    boost::asio::ip::tcp::acceptor acceptor_;
};

server::server(const config& cfg,
               invio::core::logger& logger,
               boost::asio::io_context& ctx) :
    cfg_(cfg), logger_{logger}, ctx_{ctx}, socket_{ctx}
{
    LOG_INFO(logger_, "Starting RTSP Server...");

    boost::asio::ip::tcp::endpoint endpoint{boost::asio::ip::address_v4::any(),
                                            cfg_.port};
    listener_ = std::make_shared<listener>(logger, ctx, endpoint);
}

}  // namespace invio::core::rtsp