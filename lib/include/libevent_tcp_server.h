#ifndef MNRPC_LIBEVENT_TCP_SERVER_H
#define MNRPC_LIBEVENT_TCP_SERVER_H

#include <event2/listener.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "libevent_event_loop.h"
#include "libevent_inet_address.h"
#include "libevent_tcp_connection.h"
#include "libevent_event_loop_thread_pool.h"

namespace MINIRPC {

    class LibeventTcpServer {
    public:
        LibeventTcpServer(LibeventEventLoop *loop,
                          const LibeventInetAddress &listenAddr,
                          const std::string &nameArg);

        ~LibeventTcpServer();

        void start();

        void setThreadNum(int numThreads);

        void setConnectionCallback(const std::function<void(const std::shared_ptr<LibeventTcpConnection> &)> &cb);

        void setMessageCallback(const std::function<void(const std::shared_ptr<LibeventTcpConnection> &,
                                                         struct evbuffer *,
                                                         size_t)> &cb);

    private:
        static void acceptCallback(struct evconnlistener *listener, evutil_socket_t fd,
                                   struct sockaddr *address, int socklen, void *ctx);

        void removeConnection(const std::shared_ptr<LibeventTcpConnection> &conn);

        void removeConnectionInLoop(const std::shared_ptr<LibeventTcpConnection> &conn);

        LibeventEventLoop *loop_;
        std::string name_;
        struct evconnlistener *listener_;
        LibeventInetAddress listenAddr_;
        std::unordered_map<std::string, std::shared_ptr<LibeventTcpConnection>> connections_;
        std::function<void(const std::shared_ptr<LibeventTcpConnection> &)> connectionCallback_;
        std::function<void(const std::shared_ptr<LibeventTcpConnection> &, struct evbuffer *, size_t)> messageCallback_;
        std::unique_ptr<LibeventEventLoopThreadPool> threadPool_;
    };

}
#endif //MNRPC_LIBEVENT_TCP_SERVER_H
