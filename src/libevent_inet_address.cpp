#include "libevent_inet_address.h"
#include <sstream>
#include <cstring>

namespace MINIRPC {

    LibeventInetAddress::LibeventInetAddress(uint16_t port, bool loopbackOnly) {
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
        addr_.sin_addr.s_addr = htonl(ip);
        addr_.sin_port = htons(port);
    }

    LibeventInetAddress::LibeventInetAddress(const std::string &ip, uint16_t port) {
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
        addr_.sin_port = htons(port);
    }

    LibeventInetAddress::LibeventInetAddress(const sockaddr_in &addr) : addr_(addr) {}

    std::string LibeventInetAddress::toIp() const {
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr_.sin_addr, buf, INET_ADDRSTRLEN);
        return std::string(buf);
    }

    std::string LibeventInetAddress::toPort() const {
        std::ostringstream oss;
        oss << ntohs(addr_.sin_port);
        return oss.str();
    }

    std::string LibeventInetAddress::toIpPort() const {
        std::ostringstream oss;
        oss << toIp() << ":" << toPort();
        return oss.str();
    }

    const sockaddr_in &LibeventInetAddress::getSockAddrInet() const {
        return addr_;
    }

    void LibeventInetAddress::setSockAddrInet(const sockaddr_in &addr) {
        addr_ = addr;
    }
}