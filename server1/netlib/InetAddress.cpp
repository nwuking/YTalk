/**
 * @file InetAddress.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "InetAddress.h"
#include "SocketsOps.h"
#include "Endian.h"
#include "../base/Logger.h"

namespace YTalk
{

using namespace base;

namespace netlib
{

static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

InetAddress::InetAddress(uint16_t port, bool loopbackOnly/* = false*/) {
    memset(&m_addr, 0, sizeof m_addr);
    m_addr.sin_family = AF_INET;
    in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
    m_addr.sin_addr.s_addr = sockets::hostToNetwork32(ip);
    m_addr.sin_port = sockets::hostToNetwork16(port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port) {
    memset(&m_addr, 0, sizeof m_addr);
    sockets::fromIpPort(ip.c_str(), port, &m_addr);
}

std::string InetAddress::toIpPort() const {
    char buf[32];
    sockets::toIpPort(buf, sizeof buf, m_addr);
    return buf;
}

std::string InetAddress::toIp() const {
    char buf[32];
    sockets::toIp(buf, sizeof buf, m_addr);
    return buf;
}

uint16_t InetAddress::toPort() const {
    return sockets::networkToHost16(m_addr.sin_port);
}

static thread_local char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(const std::string& hostname, InetAddress* out) {
    //assert(out != NULL);
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    memset(&hent, 0, sizeof(hent));

    int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);
    if (ret == 0 && he != NULL) {
        out->m_addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    }

    if (ret) {
        LOG_SYSERROR("InetAddress::resolve");
    }

    return false;
}

}  // namespace netlib

}  // namespace YTalk