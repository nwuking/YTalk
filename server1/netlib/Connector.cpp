/**
 * @file Connector.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Connector.h"

#include <functional>
#include <errno.h>
#include <cstring>
#include "EventLoop.h"
#include "Socket.h"
#include "SocketsOps.h"
#include "../base/Logger.h"
#include "Channel.h"

namespace YTalk
{

using namespace base;

namespace netlib
{


const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
    : m_loopPtr(loop),
    m_serverAddr(serverAddr),
    m_connect(false),
    m_state(kDisconnected),
    m_retryDelayMs(kInitRetryDelayMs)
{
    //TODO
}

Connector::~Connector() {
    //TODO
}

void Connector::start() {
    m_connect = true;
    m_loopPtr->runInLoop(std::bind(&Connector::startInLoop, this)); 
}

void Connector::startInLoop() {
    m_loopPtr->assertInLoopThread();

    if (m_state != kDisconnected)
        return;

    if (m_connect) {
        connect();
    }
    else {
        LOG_TRACE("do not connect");
    }
}

void Connector::stop() {
    m_connect = false;
    m_loopPtr->queueInLoop(std::bind(&Connector::stopInLoop, shared_from_this())); // FIXME: unsafe
}

void Connector::stopInLoop() {
    m_loopPtr->assertInLoopThread();
    if (m_state == kConnecting) {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::connect() {
    int sockfd = sockets::createNonblockingOrDie();
    int ret = sockets::connect(sockfd, m_serverAddr.getSockAddrInet());

    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno) {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
        connecting(sockfd);
        break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
        retry(sockfd);
        break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
        LOG_SYSERROR("connect error in Connector::startInLoop, %d ", savedErrno);
        sockets::close(sockfd);
        break;

    default:
        LOG_SYSERROR("Unexpected error in Connector::startInLoop, %d ", savedErrno);
        sockets::close(sockfd);
        // connectErrorCallback_();
        break;
    }
}

void Connector::restart() {
    m_loopPtr->assertInLoopThread();
    setState(kDisconnected);
    m_retryDelayMs = kInitRetryDelayMs;
    m_connect = true;
    startInLoop();
}

void Connector::connecting(int sockfd) {
    setState(kConnecting);
    //assert(!channel_);
    m_channelPtr.reset(new Channel(m_loopPtr, sockfd));
    m_channelPtr->setWriteCallback(std::bind(&Connector::handleWrite, this)); 
    m_channelPtr->setErrorCallback(std::bind(&Connector::handleError, this));

    m_channelPtr->enableWriting();
}

int Connector::removeAndResetChannel() {
    m_channelPtr->disableAll();
    m_channelPtr->remove();
    int sockfd = m_channelPtr->fd();
    m_loopPtr->queueInLoop(std::bind(&Connector::resetChannel, shared_from_this())); 
    return sockfd;
}

void Connector::resetChannel() {
    m_channelPtr.reset();
}

void Connector::handleWrite() {
    LOG_TRACE("Connector::handleWrite %d", m_state);

    if (m_state == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if (err) {
            LOG_WRANING("Connector::handleWrite - SO_ERROR = %d %s", err, strerror(err));
            retry(sockfd);
        }
        else if (sockets::isSelfConnect(sockfd)) {
            LOG_WRANING("Connector::handleWrite - Self connect");
            retry(sockfd);
        }
        else {
            setState(kConnected);
            if (m_connect) {
                //newConnectionCallback_指向TcpClient::newConnection(int sockfd)
                m_newConnectionCallBack(sockfd);
            }
            else {
                sockets::close(sockfd);
            }
        }
    }
    else {
        if (m_state != kDisconnected)
            LOG_SYSERROR("state_ != kDisconnected");
    }
}

void Connector::handleError() {
    LOG_ERROR("Connector::handleError state=%d", m_state);
    if (m_state == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        LOG_TRACE("SO_ERROR = %d %s", err, strerror(err));
        LOG_ERROR("Connector::handleError state=%d", m_state);
        retry(sockfd);
    }
}

void Connector::retry(int sockfd) {
    sockets::close(sockfd);
    setState(kDisconnected);
    if (m_connect) {
        LOG_INFO("Connector::retry - Retry connecting to %s in %d  milliseconds.", m_serverAddr.toIpPort().c_str(), m_retryDelayMs);

    }
    else {
        LOG_TRACE("do not connect");
    }
}

    
}   /// namespce netlib

}   // namespace YTalk