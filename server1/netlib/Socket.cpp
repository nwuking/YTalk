/**
 * @file Socket.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Socket.h"
#include "SocketsOps.h"
#include "InetAddress.h"

namespace YTalk
{

namespace netlib
{

Socket::~Socket() {
    sockets::close(m_sockfd);
}

void Socket::bindAddress(const InetAddress& addr) {
    sockets::bindOrDie(m_sockfd, addr.getSockAddrInet());
}

void Socket::listen() {
    sockets::listenOrDie(m_sockfd);
}

int Socket::accept(InetAddress* peeraddr) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    int connfd = sockets::accept(m_sockfd, &addr);
    if (connfd >= 0)
    {
        peeraddr->setSockAddrInet(addr);
    }
    return connfd;
}

void Socket::shutdownWrite() {
    sockets::shutdownWrite(m_sockfd);
}

void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setReuseAddr(bool on) {
    sockets::setReuseAddr(m_sockfd, on);
}

void Socket::setReusePort(bool on) {
    sockets::setReusePort(m_sockfd, on);
}

void Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));

}

}  // namespace netlib

}  // namespace YTalk