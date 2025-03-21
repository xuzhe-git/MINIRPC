#ifndef MNRPC_LIBEVENT_TCP_CONNECTION_H
#define MNRPC_LIBEVENT_TCP_CONNECTION_H

#include <event2/bufferevent.h>
#include <functional>
#include <memory>
#include <string>
#include "libevent_event_loop.h"
#include "libevent_inet_address.h"

namespace MINIRPC {

    class LibeventTcpConnection : public std::enable_shared_from_this<LibeventTcpConnection> {
    public:
        enum StateE {
            kDisconnected, kConnecting, kConnected, kDisconnecting
        };

        LibeventTcpConnection(LibeventEventLoop *loop,
                              const std::string &name,
                              evutil_socket_t sockfd,
                              const LibeventInetAddress &localAddr,
                              const LibeventInetAddress &peerAddr);

        ~LibeventTcpConnection();

        LibeventEventLoop *getLoop() const;

        const std::string &name() const;

        const LibeventInetAddress &localAddress() const;

        const LibeventInetAddress &peerAddress() const;

        bool connected() const;

        void setConnectionCallback(const std::function<void(const std::shared_ptr<LibeventTcpConnection> &)> &cb);

        void setMessageCallback(const std::function<void(const std::shared_ptr<LibeventTcpConnection> &,
                                                         struct evbuffer *,
                                                         size_t)> &cb);

        void setCloseCallback(const std::function<void(const std::shared_ptr<LibeventTcpConnection> &)> &cb);

        void connectEstablished();

        void connectDestroyed();

        void send(const std::string &message);

    private:
        static void readCallback(struct bufferevent *bev, void *ctx);

        static void eventCallback(struct bufferevent *bev, short events, void *ctx);

        LibeventEventLoop *loop_;
        std::string name_;
        StateE state_;
        evutil_socket_t sockfd_;
        struct bufferevent *bev_;
        LibeventInetAddress localAddr_;
        LibeventInetAddress peerAddr_;
        std::function<void(const std::shared_ptr<LibeventTcpConnection> &)> connectionCallback_;
        std::function<void(const std::shared_ptr<LibeventTcpConnection> &, struct evbuffer *, size_t)> messageCallback_;
        std::function<void(const std::shared_ptr<LibeventTcpConnection> &)> closeCallback_;
    };
}

#endif //MNRPC_LIBEVENT_TCP_CONNECTION_H