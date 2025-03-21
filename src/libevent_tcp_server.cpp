#include "libevent_tcp_server.h"
#include <iostream>
#include <cassert>

namespace MINIRPC {

    LibeventTcpServer::LibeventTcpServer(LibeventEventLoop *loop,
                                         const LibeventInetAddress &listenAddr,
                                         const std::string &nameArg)
            : loop_(loop), name_(nameArg), listenAddr_(listenAddr) {
        struct sockaddr_in sin = listenAddr.getSockAddrInet();
        listener_ = evconnlistener_new_bind(loop->getEventBase(), acceptCallback, this,
                                            LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                            -1, (struct sockaddr *) &sin, sizeof(sin));
        if (!listener_) {
            std::cerr << "Failed to create listener!" << std::endl;
        }
    }

    LibeventTcpServer::~LibeventTcpServer() {
        if (listener_) {
            evconnlistener_free(listener_);
        }
    }

    void LibeventTcpServer::start() {
        loop_->assertInLoopThread();
        if (!threadPool_) {
            threadPool_ = std::make_unique<LibeventEventLoopThreadPool>(loop_, 0);
        }
        threadPool_->start();
        if (listener_) {
            evconnlistener_enable(listener_);
        }
    }

    void LibeventTcpServer::setThreadNum(int numThreads) {
        loop_->assertInLoopThread();
        threadPool_ = std::make_unique<LibeventEventLoopThreadPool>(loop_, numThreads);
    }

    void LibeventTcpServer::setConnectionCallback(
            const std::function<void(const std::shared_ptr<LibeventTcpConnection> &)> &cb) {
        connectionCallback_ = cb;
    }

    void LibeventTcpServer::setMessageCallback(const std::function<void(const std::shared_ptr<LibeventTcpConnection> &,
                                                                        struct evbuffer *,
                                                                        size_t)> &cb) {
        messageCallback_ = cb;
        std::cout << "Message callback set for server: " << name_ << std::endl;
    }

    void LibeventTcpServer::acceptCallback(struct evconnlistener *listener, evutil_socket_t fd,
                                           struct sockaddr *address, int socklen, void *ctx) {
        auto self = static_cast<LibeventTcpServer *>(ctx);
        LibeventInetAddress peerAddr(*reinterpret_cast<sockaddr_in *>(address));
        std::string connName = self->name_ + "-" + peerAddr.toIpPort();
        LibeventEventLoop *ioLoop = self->threadPool_->getNextLoop();
        auto conn = std::make_shared<LibeventTcpConnection>(ioLoop, connName, fd, self->listenAddr_, peerAddr);
        conn->setConnectionCallback(self->connectionCallback_);
        conn->setMessageCallback(self->messageCallback_);
        conn->setCloseCallback([self](const std::shared_ptr<LibeventTcpConnection> &conn) {
            self->removeConnection(conn);
        });
        self->connections_[connName] = conn;
        ioLoop->runInLoop([conn]() {
            conn->connectEstablished();
        });
    }

    void LibeventTcpServer::removeConnection(const std::shared_ptr<LibeventTcpConnection> &conn) {
        conn->getLoop()->runInLoop([this, conn]() {
            removeConnectionInLoop(conn);
        });
    }

    void LibeventTcpServer::removeConnectionInLoop(const std::shared_ptr<LibeventTcpConnection> &conn) {
        conn->getLoop()->assertInLoopThread();
        auto it = connections_.find(conn->name());
        if (it != connections_.end()) {
            size_t n = connections_.erase(conn->name());
            assert(n == 1);
            conn->connectDestroyed();
        }
    }
}