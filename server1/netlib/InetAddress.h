/**
 * @file InetAddress.h
 * @author nwuking@qq.com
 * @brief 网络地址的封装
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_INET_ADDRESS_H
#define YTALK_INET_ADDRESS_H

#include <string>

#include "Types.h"

namespace YTalk
{

namespace netlib
{

class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);


    InetAddress(const std::string& ip, uint16_t port);

    InetAddress(const struct sockaddr_in& addr)
        : m_addr(addr)
    { }

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const struct sockaddr_in& getSockAddrInet() const { return m_addr; }
    void setSockAddrInet(const struct sockaddr_in& addr) { m_addr = addr; }

    uint32_t ipNetEndian() const { return m_addr.sin_addr.s_addr; }
    uint16_t portNetEndian() const { return m_addr.sin_port; }

    static bool resolve(const std::string& hostname, InetAddress* result);
private:
    struct sockaddr_in      m_addr;

};

}  // namespace netlib

}  // namespace YTalk

#endif //YTALK_INET_ADDRESS_H