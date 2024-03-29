#include "core/net/tcp_acceptor.hpp"
#include "core/net/tcp_socket.hpp"

#include "core/logger_manager.hpp"
#include "support/error.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/system/error_code.hpp>

namespace invio::core::net {

class tcp_acceptor::impl : public std::enable_shared_from_this<impl>
{
public:
    impl(boost::asio::io_context& ctx,
         const boost::asio::ip::tcp::endpoint& endpoint,
         tcp_acceptor_handler& handler, invio::core::logger& logger) :
        acceptor_{ctx}, handler_{handler}, logger_{logger}
    {
        listen(endpoint);
    }

    ~impl() { stop(); }

    void start() { accept(); }

    void stop() 
    { 
        acceptor_.close();
        started_ = false;
    }

private:
    void listen(const boost::asio::ip::tcp::endpoint& endpoint)
    {
        try
        {
            acceptor_.open(endpoint.protocol());
            acceptor_.bind(endpoint);
            acceptor_.listen();
            started_ = true;

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
        if (!started_)
            return;

        if (ec == boost::asio::error::operation_aborted)
            return;

        if (ec)
        {
            LOG_ERR(logger_, "Cannot accept connection (error: {})",
                    ec.message());
        }
        else
        {
            handler_.on_accepted(socket);
        }

        accept();
    }

private:
    boost::asio::ip::tcp::acceptor acceptor_;
    tcp_acceptor_handler& handler_;
    invio::core::logger& logger_;
    bool started_{false};
};

tcp_acceptor::tcp_acceptor(boost::asio::io_context& ctx,
                           invio::core::logger& logger) :
    ctx_{ctx}, logger_{logger}
{
}

void tcp_acceptor::start(const boost::asio::ip::tcp::endpoint& endpoint,
                         tcp_acceptor_handler& handler)
{
    INVIO_ASSERT(!impl_, "Acceptor already started");
    impl_ = std::make_shared<impl>(ctx_, endpoint, handler, logger_);
    impl_->start();
}

void tcp_acceptor::stop()
{
    INVIO_ASSERT(impl_, "Acceptor is not started");
    if (impl_)
        impl_->stop();
    impl_ = nullptr;
}

}  // namespace invio::core::net
