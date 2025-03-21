#include <iostream>
#include "libevent_event_loop.h"
#include "libevent_inet_address.h"
#include "libevent_tcp_server.h"
#include <event2/buffer.h>

using namespace MINIRPC;


void onConnection(const std::shared_ptr<LibeventTcpConnection>& conn) {
    if (conn->connected()) {
        std::cout << "New connection from " << conn->peerAddress().toIpPort() << std::endl;
    } else {
        std::cout << "Connection closed: " << conn->peerAddress().toIpPort() << std::endl;
    }
}


void onMessage(const std::shared_ptr<LibeventTcpConnection>& conn,
               struct evbuffer* buffer,
               size_t len) {
    char data[len + 1];
    evbuffer_remove(buffer, data, len);
    data[len] = '\0';
    std::cout << "Received message: " << data << " from " << conn->peerAddress().toIpPort() << std::endl;

    conn->send(data);
}

int main() {
    LibeventEventLoop loop;
    LibeventInetAddress listenAddr(8888);
    LibeventTcpServer server(&loop, listenAddr, "EchoServer");

    server.setThreadNum(4);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.start();
    loop.loop();

    return 0;
}