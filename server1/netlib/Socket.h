/**
 * @file Socket.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_SOCKET_H
#define YTALK_SOCKET_H

#include "Types.h"

namespace YTalk
{

namespace netlib
{

class InetAddress;

class Socket
{
public:
    explicit Socket(int sockfd) : m_sockfd(sockfd) { }
    ~Socket();

    SOCKET fd() const { return m_sockfd; }

    void bindAddress(const InetAddress& localaddr);
    void listen();

    int accept(InetAddress* peeraddr);

    void shutdownWrite();
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const SOCKET m_sockfd;
};   // class Socket

}  // namespace netlib

}  // namespace YTalk

#endif   // YTALK_SOCKET_H