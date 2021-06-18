#include "core/net/udp_socket.hpp"

#include "core/logger_manager.hpp"
#include "support/assert.hpp"
#include "utils/memory_arena.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/system/error_code.hpp>

#include <deque>
#include <memory>
#include <vector>

namespace {
constexpr static std::size_t buffer_size = 8 * 1024;  // 8kB
constexpr static std::size_t initial_arena_size = 2;
}  // namespace

using arena_type = invio::utils::growable_memory_arena<std::vector<std::byte>>;

namespace invio::core::net {

class udp_socket::impl : public std::enable_shared_from_this<impl>
{
public:
    impl(boost::asio::io_context& ctx, udp_socket_handler& handler,
         invio::core::logger& logger) :
        socket_{ctx_, boost::asio::ip::udp::endpoint{}},
        ctx_{ctx},
        handler_{&handler},
        logger_{logger},
        send_arena_{
            std::make_shared<arena_type>(initial_arena_size, buffer_size)}
    {
        start();
    }

    ~impl()
    {
        stop();
    }

    void bind(const boost::asio::ip::udp::endpoint& endpoint)
    {
        socket_.bind(endpoint);
    }

    // Send pending messages
    void close()
    {
        LOG_INFO(logger_, "Closing UDP socket...");

        if (stopped())
            return;

        if (send_queue_.empty())
            stop();

        state_ = state::closing;
        socket_.shutdown(boost::asio::socket_base::shutdown_receive);
    }

    // Stop without sending pending messages
    void stop()
    {
        if (stopped())
            return;

        LOG_INFO(logger_, "Stopping UDP socket...");

        state_ = state::stopped;

        socket_.shutdown(boost::asio::ip::udp::socket::shutdown_both);
        socket_.close();
    }

    void send_to(std::span<const std::byte> data,
        const boost::asio::ip::udp::endpoint& dest)
    {
        if (stopped())
            return;

        auto buf = send_arena_->get(data.size());
        std::memcpy(buf->data(), data.data(), data.size());

        const auto can_send_next_data = send_queue_.empty();
        send_queue_.push_back({std::move(buf), dest});
        if (can_send_next_data)
            do_send();
    }

    void clear_handler() { handler_ = nullptr; }

private:
    enum class state
    {
        stopped,
        started,
        closing,
    };

private:
    void start()
    {
        if (!stopped())
            return;

        LOG_INFO(logger_, "UDP socket started");

        state_ = state::started;
        read();
    }

    bool stopped() const { return state_ == state::stopped; }

    void read()
    {
        socket_.async_receive_from(
            boost::asio::buffer(buffer_), remote_endpoint_,
            [self = this->shared_from_this()](const auto& ec, auto size) {
                self->on_read(ec, size);
            });
    }

    void do_send()
    {
        if (stopped())
            return;

        INVIO_ASSERT(!send_queue_.empty(), "Send queue is empty");

        const auto data = *send_queue_.front().data;
        const auto dest = send_queue_.front().dest;

        socket_.async_send_to(
            boost::asio::buffer(data), dest,
            [self = shared_from_this(), dest](const auto& ec, auto size) {
                self->on_sent(ec, size, dest);
            });
    }

    void on_read(const boost::system::error_code& ec, std::size_t size)
    {
        if (stopped())
            return;

        if (ec == boost::asio::error::operation_aborted)
            return;

        if (ec)
        {
            LOG_ERR(
                logger_,
                "Error when reading (remote endpoint: {}:{}, error code: {}, "
                "error: {})",
                remote_endpoint_.address().to_string(), remote_endpoint_.port(),
                ec.value(), ec.message());
        }

        if (!ec && handler_)
        {
            handler_->received(std::span{buffer_.data(), buffer_.size()},
                               remote_endpoint_);
        }

        read();
    }

    void on_sent(const boost::system::error_code& ec, std::size_t size,
                 const boost::asio::ip::udp::endpoint& dest)
    {
        if (ec == boost::asio::error::operation_aborted)
            return;

        // If in meantime appears stop
        if (stopped())
            return;

        if (ec)
        {
            LOG_ERR(logger_,
                    "Error when sending (destinaion: {}:{}, error code: {}, "
                    "error: {})",
                    dest.address().to_string(), dest.port(), ec.value(),
                    ec.message());
        }
        send_queue_.pop_front();

        if (handler_)
            handler_->sent(!ec, dest);

        if (!send_queue_.empty())
            do_send();
        else if (state_ == state::closing)
            stop();
    }

private:
    struct packet_to_send
    {
        arena_type::item data;
        boost::asio::ip::udp::endpoint dest;
    };

private:
    boost::asio::ip::udp::socket socket_;
    boost::asio::io_context& ctx_;
    boost::asio::ip::udp::endpoint remote_endpoint_{};
    udp_socket_handler* handler_;
    invio::core::logger& logger_;

    std::vector<std::byte> buffer_{buffer_size};
    std::shared_ptr<arena_type> send_arena_;
    std::deque<packet_to_send> send_queue_;

    state state_{state::stopped};
};

udp_socket::udp_socket(boost::asio::io_context& ctx,
                       udp_socket_handler& handler,
                       invio::core::logger& logger) :
    ctx_{ctx}, handler_{handler}, logger_{logger}
{
}

udp_socket::~udp_socket()
{
    if (impl_)
    {
        impl_->clear_handler();
        stop(false);
    }
}

void udp_socket::start()
{
    INVIO_ASSERT(!impl_, "UDP socket already started");
    if (impl_)
        return;

    impl_ = std::make_shared<impl>(ctx_, handler_, logger_);
}

void udp_socket::stop(bool send_pending)
{
    INVIO_ASSERT(impl_, "UDP socket is not started");
    if (!impl_)
        return;
    
    if (send_pending)
        impl_->close();
    else
        impl_->stop();
}

void udp_socket::bind(const boost::asio::ip::udp::endpoint& ep)
{
    INVIO_ASSERT(impl_, "UDP socket is not started");
    if (!impl_)
        return;

    impl_->bind(ep);
}

void udp_socket::send_to(std::span<const std::byte> data,
                         const boost::asio::ip::udp::endpoint& dest)
{
    INVIO_ASSERT(impl_, "UDP socket is not started");
    if (!impl_)
        return;

    impl_->send_to(data, dest);
}

}  // namespace invio::core::net