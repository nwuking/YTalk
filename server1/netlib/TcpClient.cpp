/**
 * @file TcpClient.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "TcpClient.h"
#include "../base/Logger.h"
#include "Types.h"
#include "EventLoop.h"
#include "Connector.h"
#include "TcpConnection.h"

#include <cstdio>
#include <functional>

namespace YTalk
{

using namespace base;

namespace netlib
{

namespace detail
{

        void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn) {
            loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
        }

        void removeConnector(const CONNECTOR_PTR& connector) {
            //connector->
        }

}   /// namespace detail

TcpClient::TcpClient(EventLoop* loop,
    const InetAddress& serverAddr,
    const std::string& nameArg)
    : m_loopPtr(loop),
    m_connectorPtr(new Connector(loop, serverAddr)),
    m_name(nameArg),
    m_connectionCallBack(defaultConnectionCallback),
    m_messageCallBack(defaultMessageCallback),
    m_retry(false),
    m_connect(true),
    m_nextConnId(1)
{
    m_connectorPtr->setNewConnectionCallback(
        std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
    LOG_TRACE("TcpClient::TcpClient[%s] - connector 0x%x", m_name.c_str(), m_connectorPtr.get());
}

TcpClient::~TcpClient() {
    LOG_TRACE("TcpClient::~TcpClient[%s] - connector 0x%x", m_name.c_str(), m_connectorPtr.get());
    TcpConnectionPtr conn;
    bool unique = false;
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        unique = m_connectionPtr.unique();
        conn = m_connectionPtr;
    }
    if (conn) {
        //assert(loop_ == conn->getLoop());
        if (m_loopPtr != conn->getLoop())
            return;

        // FIXME: not 100% safe, if we are in different thread
        CloseCallBack cb = std::bind(&detail::removeConnection, m_loopPtr, std::placeholders::_1);
        m_loopPtr->runInLoop(std::bind(&TcpConnection::setCloseCallBack, conn, cb));
        if (unique) {
            conn->forceClose();
        }
    }
    else {
        m_connectorPtr->stop();
        // FIXME: HACK
        // loop_->runAfter(1, boost::bind(&detail::removeConnector, connector_));
    }
}

void TcpClient::connect() {
    // FIXME: check state
    LOG_TRACE("TcpClient::connect[%s] - connecting to %s", m_name.c_str(), m_connectorPtr->serverAddress().toIpPort().c_str());
    m_connect = true;
    m_connectorPtr->start();
}

void TcpClient::disconnect() {
    m_connect = false;

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_connectionPtr) {
            m_connectionPtr->shutdown();
        }
    }
}

void TcpClient::stop() {
    m_connect = false;
    m_connectorPtr->stop();
}

void TcpClient::newConnection(int sockfd) {
    m_loopPtr->assertInLoopThread();
    InetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toIpPort().c_str(), m_nextConnId);
    ++m_nextConnId;
    std::string connName = m_name + buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    TcpConnectionPtr conn(new TcpConnection(m_loopPtr, connName, sockfd, localAddr, peerAddr));
    conn->setConnectionCallBack(m_connectionCallBack);
    conn->setMessageCallBack(m_messageCallBack);
    conn->setWriteCompleteCallBack(m_writeCompleteCallBack);
    conn->setCloseCallBack(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1)); // FIXME: unsafe

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_connectionPtr = conn;
    }

    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn) {
    m_loopPtr->assertInLoopThread();
    //assert(loop_ == conn->getLoop());
    if (m_loopPtr != conn->getLoop())
        return;

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        //assert(connection_ == conn);
        if (m_connectionPtr != conn)
            return;

        m_connectionPtr.reset();
    }

    m_loopPtr->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if (m_retry && m_connect) {
        LOG_TRACE("TcpClient::connect[%s] - Reconnecting to %s", m_name.c_str(), m_connectorPtr->serverAddress().toIpPort().c_str());
        m_connectorPtr->restart();
    }
}
    
}   /// namespce netlib

}   // namespace YTalk