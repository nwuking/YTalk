/**
 * @file SocketsOps.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "SocketsOps.h"
#include "../base/Logger.h"
#include "Endian.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

namespace YTalk
{

using namespace base;

namespace netlib
{

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr)
{
    return static_cast<const struct sockaddr*>((const void*)(addr));
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in* addr)
{
    return static_cast<struct sockaddr*>((void*)(addr));
}

const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr)
{
    return static_cast<const struct sockaddr_in*>((const void*)(addr));
}

struct sockaddr_in* sockets::sockaddr_in_cast(struct sockaddr* addr)
{
    return static_cast<struct sockaddr_in*>((void*)(addr));
}

SOCKET sockets::createOrDie()
{
    SOCKET sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET)
    {
        LOG_FATAL("sockets::createNonblockingOrDie");
    }

    return sockfd;
}

SOCKET sockets::createNonblockingOrDie()
{
    SOCKET sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET)
    {
        LOG_FATAL("sockets::createNonblockingOrDie");
    }

    setNonBlockAndCloseOnExec(sockfd);
    return sockfd;
}

void sockets::setNonBlockAndCloseOnExec(SOCKET sockfd)
{
    // non-block
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);
    // FIXME check

    // close-on-exec
    flags = ::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFD, flags);
    // FIXME check

    (void)ret;       
}

void sockets::bindOrDie(SOCKET sockfd, const struct sockaddr_in& addr)
{
    int ret = ::bind(sockfd, sockaddr_cast(&addr), static_cast<socklen_t>(sizeof addr));
    if (ret == SOCKET_ERROR)
    {
        LOG_FATAL("sockets::bindOrDie");
    }
}

void sockets::listenOrDie(SOCKET sockfd)
{
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret == SOCKET_ERROR)
    {
        LOG_FATAL("sockets::listenOrDie");
    }
}

SOCKET sockets::accept(SOCKET sockfd, struct sockaddr_in* addr)
{
    socklen_t addrlen = static_cast<socklen_t>(sizeof * addr);
    SOCKET connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd == SOCKET_ERROR)
    {
        int savedErrno = errno;
        LOG_SYSERROR("Socket::accept");
        switch (savedErrno)
        {
        case EAGAIN:
        case ECONNABORTED:
        case EINTR:
        case EPROTO: // ???
        case EPERM:
        case EMFILE: // per-process lmit of open file desctiptor ???
            // expected errors
            errno = savedErrno;
            break;
        case EBADF:
        case EFAULT:
        case EINVAL:
        case ENFILE:
        case ENOBUFS:
        case ENOMEM:
        case ENOTSOCK:
        case EOPNOTSUPP:
            // unexpected errors
            LOG_FATAL("unexpected error of ::accept %d", savedErrno);
            break;
        default:
            LOG_FATAL("unknown error of ::accept %d", savedErrno);
            break;
        }
    }

    return connfd;
}

void sockets::setReuseAddr(SOCKET sockfd, bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
}

void sockets::setReusePort(SOCKET sockfd, bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
    if (ret < 0 && on)
    {
        LOG_SYSERROR("SO_REUSEPORT failed.");
    }
}

SOCKET sockets::connect(SOCKET sockfd, const struct sockaddr_in& addr)
{
    return ::connect(sockfd, sockaddr_cast(&addr), static_cast<socklen_t>(sizeof addr));
}

int32_t sockets::read(SOCKET sockfd, void* buf, int32_t count)
{
    return ::read(sockfd, buf, count);
}

ssize_t sockets::readv(SOCKET sockfd, const struct iovec* iov, int iovcnt)
{
    return ::readv(sockfd, iov, iovcnt);
}

int32_t sockets::write(SOCKET sockfd, const void* buf, int32_t count)
{
    return ::write(sockfd, buf, count);

}

void sockets::close(SOCKET sockfd)
{
    if (::close(sockfd) < 0)
    {
        LOG_SYSERROR("sockets::close, fd=%d, errno=%d, errorinfo=%s", sockfd, errno, strerror(errno));
    }
}

void sockets::shutdownWrite(SOCKET sockfd)
{
    if (::shutdown(sockfd, SHUT_WR) < 0)        
    {
        LOG_SYSERROR("sockets::shutdownWrite");
    }
}

void sockets::toIpPort(char* buf, size_t size, const struct sockaddr_in& addr)
{
    //if (size >= sizeof(struct sockaddr_in))
    //    return;

    ::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
    size_t end = ::strlen(buf);
    uint16_t port = sockets::networkToHost16(addr.sin_port);
    //if (size > end)
    //    return;

    snprintf(buf + end, size - end, ":%u", port);
}

void sockets::toIp(char* buf, size_t size, const struct sockaddr_in& addr)
{
    if (size >= sizeof(struct sockaddr_in))
        return;

    ::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
}

void sockets::fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr)
{
    addr->sin_family = AF_INET;
    //TODO: 校验下写的对不对
    addr->sin_port = htobe16(port);
    if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
    {
        LOG_SYSERROR("sockets::fromIpPort");
    }
}

int sockets::getSocketError(SOCKET sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
        return errno;
    return optval;
}

struct sockaddr_in sockets::getLocalAddr(SOCKET sockfd)
{
    struct sockaddr_in localaddr = { 0 };
    memset(&localaddr, 0, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    ::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen);
    return localaddr;
}

struct sockaddr_in sockets::getPeerAddr(SOCKET sockfd)
{
    struct sockaddr_in peeraddr = { 0 };
    memset(&peeraddr, 0, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    ::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen);
    return peeraddr;
}

bool sockets::isSelfConnect(SOCKET sockfd)
{
    struct sockaddr_in localaddr = getLocalAddr(sockfd);
    struct sockaddr_in peeraddr = getPeerAddr(sockfd);
    return localaddr.sin_port == peeraddr.sin_port && localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}

    
}   /// namesapce netlib

}   // namesapce YTalk