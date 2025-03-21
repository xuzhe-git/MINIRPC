#ifndef MNRPC_LIBEVENT_IENT_ADDRESS_H
#define MNRPC_LIBEVENT_IENT_ADDRESS_H


#include <string>
#include <arpa/inet.h>

namespace MINIRPC {

    class LibeventInetAddress {
    public:
        explicit LibeventInetAddress(uint16_t port = 0, bool loopbackOnly = false);

        LibeventInetAddress(const std::string &ip, uint16_t port);

        explicit LibeventInetAddress(const sockaddr_in &addr);

        std::string toIp() const;

        std::string toPort() const;

        std::string toIpPort() const;

        const sockaddr_in &getSockAddrInet() const;

        void setSockAddrInet(const sockaddr_in &addr);

    private:
        sockaddr_in addr_;
    };
}
#endif //MNRPC_LIBEVENT_IENT_ADDRESS_H
