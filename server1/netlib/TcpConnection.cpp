/**
 * @file TcpConnection.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "../base/Logger.h"
#include "SocketsOps.h"

namespace YTalk
{

using namespace base;

namespace netlib
{

void defaultConnectionCallback(const TcpConnectionPtr& conn) {
    LOG_TRACE("%s -> %s is %s", conn->localAddress().toIpPort().c_str(), 
                                conn->peerAddress().toIpPort().c_str(), 
                                (conn->connected() ? "UP" : "DOWN"));
}

void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, base::TimeStamp receiveTime) {
    buffer->retrieveAll();
}

TcpConnection::TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr) :
    m_loopPtr(loop), 
    m_name(name), 
    m_state(kConnecting), 
    m_socketPtr(new Socket(sockfd)), 
    m_channelPtr(new Channel(loop, sockfd)), 
    m_localAddr(localAddr), 
    m_peerAddr(peerAddr), 
    m_highWaterMark(64*1024*1024)
{
    m_channelPtr->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    m_channelPtr->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    m_channelPtr->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    m_channelPtr->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    LOG_DEBUG("TcpConnection::ctor[%s] at 0x%x fd=%d", m_name.c_str(), this, sockfd);
    m_socketPtr->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG("TcpConnection::dtor[%s] at 0x%x fd=%d state=%s", m_name.c_str(), this, m_channelPtr->fd(), state2String().c_str());
}

void TcpConnection::send(const void *message, int len) {
    std::string msg(static_cast<const char*>(message), len);
    send(msg);
}

void TcpConnection::send(const std::string &message) {
    if(m_state == kConnected) {
        if(m_loopPtr->isInLoopThread()) {
            sendInLoop(message);
        }
        else {
            void (TcpConnection::*fp)(const std::string &message) = &TcpConnection::sendInLoop;
            m_loopPtr->runInLoop(std::bind(fp, this, message));
        }
    }
}

void TcpConnection::send(Buffer *buffer) {
    if(m_state == kConnected) {
        if(m_loopPtr->isInLoopThread()) {
            sendInLoop(buffer->peek(), buffer->readableBytes());
            buffer->retrieveAll();
        }
        else {
            void (TcpConnection::*fp)(const std::string &message) = &TcpConnection::sendInLoop;
            m_loopPtr->runInLoop(std::bind(fp, this, buffer->retrieveAllAsString()));
        }
    }
}

void TcpConnection::sendInLoop(const std::string &message) {
    sendInLoop(message.data(), message.size());
}

void TcpConnection::sendInLoop(const void *message, int len) {
    m_loopPtr->assertInLoopThread();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;

    if(m_state == kDisconnected) {
        LOG_WRANING("disconnected, give up writing");
        return;
    }

    if(!m_channelPtr->isWriting() && m_outputBuffer.readableBytes() == 0) {
        // 尝试直接写
        nwrote = sockets::write(m_channelPtr->fd(), message, len);
        if(nwrote >= 0) {
            remaining = len - nwrote;
            if(remaining == 0 && m_writeCompleteCallBack) {
                m_loopPtr->queueInLoop(std::bind(m_writeCompleteCallBack, shared_from_this()));
            }
        }
        else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG_SYSERROR("TcpConnection::sendInLoop");
                if (errno == EPIPE || errno == ECONNRESET) {
                    faultError = true;
                }
            }
        }
    }

    if(remaining > len) {
        return;
    }

    if(!faultError && remaining > 0) {
        size_t oldLen = m_outputBuffer.readableBytes();
        if(oldLen+remaining >= m_highWaterMark && oldLen < m_highWaterMark && m_highWaterMarkCallBack) {
            m_loopPtr->queueInLoop(std::bind(m_highWaterMarkCallBack, shared_from_this(), oldLen+remaining));
        }
        m_outputBuffer.append(static_cast<const char*>(message)+nwrote, remaining);
        if(!m_channelPtr->isWriting()) {
            m_channelPtr->enableWriting();
        }
    }
}

void TcpConnection::shutdown() {
    if(m_state == kConnected) {
        setState(kDisconnecting);
        m_loopPtr->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop() {
    m_loopPtr->assertInLoopThread();

    if(!m_channelPtr->isWriting()) {
        m_socketPtr->shutdownWrite();
    }
}

void TcpConnection::forceClose() {
    if(m_state == kConnected || m_state == kDisconnecting) {
        setState(kDisconnecting);
        m_loopPtr->queueInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
    }
}

void TcpConnection::forceCloseInLoop() {
    m_loopPtr->assertInLoopThread();
    if (m_state == kConnected || m_state == kDisconnecting) {
        handleClose();
    }
}

std::string TcpConnection::state2String() {
    switch (m_state)
    {
    case kDisconnected:
        return "kDisconnected";
    case kConnecting:
        return "kConnecting";
    case kConnected:
        return "kConnected";
    case kDisconnecting:
        return "kDisconnecting";
    default:
        return "unknown state";
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    m_socketPtr->setTcpNoDelay(on);
}

void TcpConnection::connectEstablished() {
    m_loopPtr->assertInLoopThread();
    if(m_state != kConnecting) {
        return;
    }
    setState(kConnected);

    m_channelPtr->enableReading();

    m_connectionCallBack(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    m_loopPtr->assertInLoopThread();
    if(m_state == kConnected) {
        setState(kDisconnected);
        m_channelPtr->disableAll();
        m_connectionCallBack(shared_from_this());
    }
    m_channelPtr->remove();
}

void TcpConnection::handleRead(TimeStamp receiveTime) {
    m_loopPtr->assertInLoopThread();
    int saveErrno = 0;
    ssize_t n = m_inputBuffer.readFd(m_channelPtr->fd(), &saveErrno);
    if(n > 0) {
        m_messageCallBack(shared_from_this(), &m_inputBuffer, receiveTime);
    }
    else if(n == 0) {
        handleClose();
    }
    else {
        errno = saveErrno;
        LOG_SYSERROR("TcpConnection::handleRead");
        handleError();
    }
}

void TcpConnection::handleWrite() {
    m_loopPtr->assertInLoopThread();
    if(m_channelPtr->isWriting()) {
        ssize_t n = sockets::write(m_channelPtr->fd(), m_outputBuffer.peek(), m_outputBuffer.readableBytes());
        if(n > 0) {
            m_outputBuffer.retrieve(n);
            if(m_outputBuffer.readableBytes() == 0) {
                m_channelPtr->disableWriting();
                if(m_writeCompleteCallBack) {
                    m_loopPtr->queueInLoop(std::bind(m_writeCompleteCallBack, shared_from_this()));
                }
                if(m_state == kDisconnecting) {
                    shutdown();
                }
            }
        }
        else {
            LOG_SYSERROR("TcpConnection::handleWrite");
        }
    }
    else {
        LOG_TRACE("Connection fd = %d is down, no more writing", m_channelPtr->fd());
    }
}

void TcpConnection::handleClose() {
    if(m_state == kDisconnected) {
        return;
    }

    m_loopPtr->assertInLoopThread();
    LOG_TRACE("fd = %d state = %s", m_channelPtr->fd(), state2String().c_str());

    if(m_state != kConnected && m_state != kDisconnecting) {
        return;
    }
    setState(kDisconnected);
    m_channelPtr->disableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    m_connectionCallBack(guardThis);
    m_closeCallBack(guardThis);
}

void TcpConnection::handleError() {
    int err = sockets::getSocketError(m_channelPtr->fd());
    LOG_ERROR("TcpConnection::%s handleError [%d] - SO_ERROR = %s", m_name.c_str(), err, strerror(err));

    handleClose();
}
    
}   /// namespce netlib

}   // namespace YTalk