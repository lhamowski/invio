#include "tcp_socket.hpp"

#include "core/logger_manager.hpp"
#include "support/assert.hpp"
#include "utils/memory_arena.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>

#include <chrono>
#include <cstddef>
#include <cstring>
#include <deque>
#include <string>
#include <utility>
#include <vector>

namespace {
constexpr static std::size_t buffer_size = 64 * 1024;  // 64kB
constexpr static std::size_t initial_arena_size = 2;
}  // namespace

using arena_type = invio::utils::growable_memory_arena<std::vector<std::byte>>;

namespace invio::core::net {

class tcp_socket::impl : public std::enable_shared_from_this<impl>
{
public:
    impl(boost::asio::ip::tcp::socket&& socket, boost::asio::io_context& ctx,
         const tcp_socket_properties& props, tcp_socket_handler& handler,
         invio::core::logger& logger) :
        ctx_{ctx},
        socket_{std::move(socket)},
        resolver_{ctx_},
        reconnect_timer_{ctx_},
        handler_{&handler},
        props_{props},
        send_arena_{
            std::make_shared<arena_type>(initial_arena_size, buffer_size)},
        logger_{logger}
    {
        INVIO_ASSERT(socket_.is_open(), "Socket is closed");
        state_ = state::connected;
        host_ = socket_.remote_endpoint().address().to_v4().to_string();
        port_ = socket_.remote_endpoint().port();

        set_keepalive(props_.keepalive);
    }

    impl(boost::asio::io_context& ctx, const char* host, std::uint16_t port,
         const tcp_socket_properties& props, tcp_socket_handler& handler,
         invio::core::logger& logger) :
        ctx_{ctx},
        socket_{ctx_},
        resolver_{ctx_},
        reconnect_timer_{ctx_},
        handler_{&handler},
        host_{host},
        port_{port},
        props_{props},
        send_arena_{
            std::make_shared<arena_type>(initial_arena_size, buffer_size)},
        logger_{logger}
    {
    }

    void start()
    {
        if (disconnected())
            resolve();
        else
            read();
    }

    void send(std::span<const std::byte> data)
    {
        if (state_ != state::connected && state_ != state::closing)
            return;

        auto buf = send_arena_->get(data.size());
        std::memcpy(buf->data(), data.data(), data.size());

        const auto can_send_next_data = send_queue_.empty();
        send_queue_.push_back(std::move(buf));
        if (can_send_next_data)
            do_send();
    }

    void close()
    {
        switch (state_)
        {
            case state::disconnected:
            case state::closing:
                return;
            case state::reconnecting:
                stop();
                break;
            case state::connected:
                state_ = state::closing;

                // if nothing to send, just stop
                if (send_queue_.empty())
                    stop();

                socket_.shutdown(boost::asio::socket_base::shutdown_receive);
                break;
        }
    }

    void stop()
    {
        if (disconnected())
            return;

        LOG_INFO(logger_, "Stopping connection (host: {}:{})", host_, port_);

        if (state_ == state::connected)
        {
            socket_.shutdown(boost::asio::socket_base::shutdown_both);
            socket_.cancel();
        }

        socket_.close();
        state_ = state::disconnected;

        if (handler_)
            handler_->disconnected();
    }

    void set_keepalive(bool enable)
    {
        props_.keepalive = enable;
        boost::system::error_code ec;

        if (enable)
        {
            socket_.set_option(boost::asio::socket_base::keep_alive{true}, ec);
            if (ec)
            {
                LOG_ERR(logger_,
                        "Cannot enable keepalive (host: {}:{}, error code: {}, "
                        "error: {})",
                        host_, port_, ec.value(), ec.message())
            }
        }
        else
        {
            socket_.set_option(boost::asio::socket_base::keep_alive{false}, ec);
            if (ec)
            {
                LOG_ERR(logger_,
                        "Cannot disable keepalive (host: {}:{}, error code: "
                        "{}, error: {})",
                        host_, port_, ec.value(), ec.message())
            }
        }
    }

    void clear_handler() { handler_ = nullptr; }

    bool disconnected() const { return state_ == state::disconnected; }

private:
    enum class state
    {
        disconnected,
        connecting,
        connected,
        reconnecting,
        closing
    };

private:
    void resolve()
    {
        INVIO_ASSERT(state_ != state::connected, "Invalid state");
        state_ = state::connecting;

        LOG_DEBUG(logger_, "Resolving host... (host: {}:{})", host_, port_);
        resolver_.async_resolve(
            host_, std::to_string(port_),
            boost::asio::ip::resolver_base::numeric_service,
            [self = shared_from_this()](const auto& ec, const auto& res) {
                self->on_resolved(ec, res);
            });
    }

    void connect(const boost::asio::ip::tcp::resolver::results_type& res)
    {
        LOG_DEBUG(logger_, "Connecting socket to the peer endpoint...");
        boost::asio::async_connect(
            socket_, res,
            [self = shared_from_this()](const auto& ec, const auto& endpoint) {
                self->on_connected(ec, endpoint);
            });
    }

    void disconnect()
    {
        stop();

        if (props_.reconnect && reconnect_attempts_ < props_.max_reconnects)
            schedule_reconnect();
    }

    void read()
    {
        if (state_ != state::connected)
            return;

        socket_.async_read_some(
            boost::asio::mutable_buffer{recv_buffer_.get(), buffer_size},
            [self = shared_from_this()](const auto& ec, auto size) {
                self->on_read(ec, size);
            });
    }

    void do_send()
    {
        if (state_ != state::connected && state_ != state::closing)
            return;

        INVIO_ASSERT(!send_queue_.empty(), "Send queue is empty");

        boost::asio::async_write(
            socket_, boost::asio::buffer(*send_queue_.front()),
            [self = shared_from_this()](const auto& ec, auto size) {
                self->on_sent(ec, size);
            });
    }

    void schedule_reconnect()
    {
        INVIO_ASSERT(disconnected(), "Invalid state");

        state_ = state::reconnecting;
        ++reconnect_attempts_;
        LOG_DEBUG(logger_, "Reconnecting... (attempt: {}, host: {}:{})",
                  reconnect_attempts_, host_, port_);

        reconnect_timer_.expires_after(props_.reconnect_interval);
        reconnect_timer_.async_wait(
            [self = shared_from_this()](const auto& ec) {
                using clock = boost::asio::steady_timer::clock_type;

                if (self->disconnected())
                    return;
                if (ec)
                    return;
                if (self->reconnect_timer_.expiry() <= clock::now())
                    return;

                self->resolve();
            });
    }

    void on_resolved(const boost::system::error_code& ec,
                     const boost::asio::ip::tcp::resolver::results_type& res)
    {
        if (ec == boost::asio::error::operation_aborted)
            return;

        if (disconnected())
            return;

        if (ec)
        {
            LOG_ERR(
                logger_,
                "Error when resolving (host: {}:{}, error code: {}, error: {})",
                host_, port_, ec.value(), ec.message());
            disconnect();
            return;
        }

        boost::asio::async_connect(
            socket_, res,
            [self = shared_from_this()](const auto& ec, const auto& endpoint) {
                self->on_connected(ec, endpoint);
            });
    }

    void on_connected(const boost::system::error_code& ec,
                      const boost::asio::ip::tcp::endpoint& endpoint)
    {
        if (ec == boost::asio::error::operation_aborted)
            return;

        if (disconnected())
            return;

        if (ec)
        {
            LOG_ERR(logger_,
                    "Error when connecting to the peer endpoint (host: {}:{}, "
                    "endpoint: {}, error code: {}, error: {})",
                    host_, port_, endpoint.address().to_string(), ec.value(),
                    ec.message());
            disconnect();
            return;
        }

        LOG_INFO(logger_,
                 "Connected to the peer endpoint (host: {}:{}, endpoint: {})",
                 host_, port_, endpoint.address().to_string());

        state_ = state::connected;
        reconnect_attempts_ = 0;

        if (!recv_buffer_)
            recv_buffer_ = std::make_unique<std::byte[]>(buffer_size);

        if (handler_)
            handler_->connected();

        set_keepalive(props_.keepalive);
        read();
    }

    void on_read(const boost::system::error_code& ec, std::size_t size)
    {
        if (ec == boost::asio::error::operation_aborted)
            return;

        if (state_ != state::connected)
            return;

        if (ec)
        {
            LOG_ERR(
                logger_,
                "Error when reading (host: {}:{}, error code: {}, error: {})",
                host_, ec.value(), ec.message());

            disconnect();
            return;
        }

        if (size > 0 && handler_)
            handler_->received(std::span{recv_buffer_.get(), size});

        read();
    }

    void on_sent(const boost::system::error_code& ec, std::size_t size)
    {
        if (ec == boost::asio::error::operation_aborted)
            return;

        // If in meantime appears disonnection
        if (state_ != state::connected && state_ != state::closing)
            return;

        if (ec)
        {
            LOG_ERR(
                logger_,
                "Error when sending (host: {}:{}, error code: {}, error: {})",
                host_, port_, ec.value(), ec.message());
            disconnect();
            return;
        }

        send_queue_.pop_front();
        if (!send_queue_.empty())
            do_send();
        else if (state_ == state::closing)
            stop();
    }

private:
    boost::asio::io_context& ctx_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::steady_timer reconnect_timer_;
    tcp_socket_handler* handler_{};

    std::string host_{};
    std::uint16_t port_{};
    tcp_socket_properties props_{};
    state state_{state::disconnected};
    std::size_t reconnect_attempts_{};

    std::shared_ptr<arena_type> send_arena_;
    std::deque<arena_type::item> send_queue_;
    std::unique_ptr<std::byte[]> recv_buffer_;

    invio::core::logger& logger_;
};

tcp_socket::tcp_socket(boost::asio::io_context& ctx,
                       tcp_socket_handler& handler,
                       invio::core::logger& logger) :
    ctx_{ctx}, handler_{handler}, logger_{logger}
{
}

tcp_socket::~tcp_socket()
{
    if (impl_)
    {
        impl_->clear_handler();
        stop(false);
    }
}

void tcp_socket::connect(const char* host, std::uint16_t port,
                         const tcp_socket_properties& props)
{
    if (impl_)
    {
        impl_->clear_handler();
        impl_.reset();
    }

    impl_ = std::make_shared<impl>(ctx_, host, port, props, handler_, logger_);
    impl_->start();
}

void tcp_socket::connect(boost::asio::ip::tcp::socket&& socket,
                         const tcp_socket_properties& props)
{
    if (impl_)
    {
        impl_->clear_handler();
        impl_.reset();
    }

    impl_ = std::make_shared<impl>(std::move(socket), ctx_, props, handler_,
                                   logger_);
    impl_->start();
}

void tcp_socket::stop(bool send_pending)
{
    if (!impl_)
        return;

    if (send_pending)
        impl_->close();
    else
        impl_->stop();

    // We want to notify handler of disconnection in case of sending pending
    // messages
    if (impl_->disconnected())
        impl_.reset();
}

void tcp_socket::send(std::span<const std::byte> data)
{
    if (!impl_)
        return;

    impl_->send(data);
}

}  // namespace invio::core::net
