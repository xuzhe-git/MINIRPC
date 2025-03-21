#include "libevent_tcp_connection.h"
#include <iostream>
#include <cassert>
#include <event2/buffer.h>

namespace MINIRPC {

    LibeventTcpConnection::LibeventTcpConnection(LibeventEventLoop *loop,
                                                 const std::string &name,
                                                 evutil_socket_t sockfd,
                                                 const LibeventInetAddress &localAddr,
                                                 const LibeventInetAddress &peerAddr)
            : loop_(loop), name_(name), state_(kConnecting), sockfd_(sockfd),
              localAddr_(localAddr), peerAddr_(peerAddr) {
        bev_ = bufferevent_socket_new(loop->getEventBase(), sockfd, BEV_OPT_CLOSE_ON_FREE);
        if (!bev_) {
            std::cerr << "Failed to create bufferevent!" << std::endl;
        }
        bufferevent_setcb(bev_, readCallback, nullptr, eventCallback, this);
    }

    LibeventTcpConnection::~LibeventTcpConnection() {
        if (bev_) {
            bufferevent_free(bev_);
        }
    }

    LibeventEventLoop *LibeventTcpConnection::getLoop() const {
        return loop_;
    }

    const std::string &LibeventTcpConnection::name() const {
        return name_;
    }

    const LibeventInetAddress &LibeventTcpConnection::localAddress() const {
        return localAddr_;
    }

    const LibeventInetAddress &LibeventTcpConnection::peerAddress() const {
        return peerAddr_;
    }

    bool LibeventTcpConnection::connected() const {
        return state_ == kConnected;
    }

    void LibeventTcpConnection::setConnectionCallback(
            const std::function<void(const std::shared_ptr<LibeventTcpConnection> &)> &cb) {
        connectionCallback_ = cb;
    }

    void
    LibeventTcpConnection::setMessageCallback(const std::function<void(const std::shared_ptr<LibeventTcpConnection> &,
                                                                       struct evbuffer *,
                                                                       size_t)> &cb) {
        messageCallback_ = cb;
    }

    void LibeventTcpConnection::setCloseCallback(
            const std::function<void(const std::shared_ptr<LibeventTcpConnection> &)> &cb) {
        closeCallback_ = cb;
    }

    void LibeventTcpConnection::connectEstablished() {
        loop_->assertInLoopThread();
        assert(state_ == kConnecting);
        state_ = kConnected;
        if (connectionCallback_) {
            connectionCallback_(shared_from_this());
        }
        bufferevent_enable(bev_, EV_READ | EV_WRITE);
    }

    void LibeventTcpConnection::connectDestroyed() {
        loop_->assertInLoopThread();
        if (state_ == kConnected) {
            state_ = kDisconnected;
            bufferevent_disable(bev_, EV_READ | EV_WRITE);
            if (connectionCallback_) {
                connectionCallback_(shared_from_this());
            }
        }
        if (closeCallback_) {
            closeCallback_(shared_from_this());
        }
    }

    void LibeventTcpConnection::send(const std::string &message) {
        if (state_ == kConnected) {
            bufferevent_write(bev_, message.data(), message.size());
        }
    }

    void LibeventTcpConnection::readCallback(struct bufferevent *bev, void *ctx) {
        auto self = static_cast<LibeventTcpConnection *>(ctx);
        struct evbuffer *input = bufferevent_get_input(bev);
        size_t len = evbuffer_get_length(input);
        if (self->messageCallback_) {
            self->messageCallback_(self->shared_from_this(), input, len);
        }
    }

    void LibeventTcpConnection::eventCallback(struct bufferevent *bev, short events, void *ctx) {
        auto self = static_cast<LibeventTcpConnection *>(ctx);
        if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
            self->connectDestroyed();
        }
    }
}