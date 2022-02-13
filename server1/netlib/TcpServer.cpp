/**
 * @file TcpServer.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "TcpServer.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "SocketsOps.h"
#include "../base/Logger.h"

#include <memory>

namespace YTalk
{

using namespace base;

namespace netlib
{

TcpServer::TcpServer(EventLoop *loop, 
                    const InetAddress &listenAddr,
                    const std::string &name,
                    Option option)
    : m_loopPtr(loop),
      m_ipPort(listenAddr.toIpPort()),
      m_name(name),
      m_acceptor(new Acceptor(loop, listenAddr, kReusePort == option)),
      m_connectionCallBack(defaultConnectionCallback),
      m_messageCallBack(defaultMessageCallback),
      m_started(0),
      m_nextConnId(1)
{
  m_acceptor->setNewConnectionCallBack(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer() {
  m_loopPtr->assertInLoopThread();
  LOG_TRACE("TcpServer::~TcpServer [%s] destruct", m_name.c_str());

  for(auto &item : m_connections) {
    TcpConnectionPtr conn(item.second);
    item.second.reset();
    conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    conn.reset();
  }
}

void TcpServer::start(int woker) {
  if(m_started == 0) {
    m_eventLoopThreadPool.reset(new EventLoopThreadPool(m_loopPtr, woker, m_name));
    m_eventLoopThreadPool->start();

    m_loopPtr->runInLoop(std::bind(&Acceptor::listen, m_acceptor.get()));
    m_started = 1;
  }
}

void TcpServer::newConnection(int sockFd, const InetAddress &peerAddr) {
  m_loopPtr->assertInLoopThread();
  EventLoop *loop = m_eventLoopThreadPool->getNextLoop();
  char buf[64];
  snprintf(buf, sizeof buf, "-%s#%d", m_ipPort.c_str(), m_nextConnId);
  ++m_nextConnId;
  std::string connName = m_name + buf;

  LOG_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s", 
            m_name.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());
  InetAddress localAddr(sockets::getLocalAddr(sockFd));
  TcpConnectionPtr conn(new TcpConnection(loop,
                                          connName,
                                          sockFd,
                                          localAddr,
                                          peerAddr));
  m_connections[connName] = conn;
  conn->setConnectionCallBack(m_connectionCallBack);
  conn->setMessageCallBack(m_messageCallBack);
  conn->setWriteCompleteCallBack(m_writeCompleteCallBack);
  conn->setCloseCallBack(
      std::bind(&TcpServer::removeConnection, this, std::placeholders::_1)); 
  loop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
  // FIXME: unsafe
  m_loopPtr->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
    m_loopPtr->assertInLoopThread();
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s", m_name.c_str(), conn->name().c_str());
    size_t n = m_connections.erase(conn->name());

    if (n != 1)
    {
        LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s, connection does not exist.", m_name.c_str(), conn->name().c_str());
        return;
    }

    EventLoop* loop = conn->getLoop();
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

}   /// namespce netlib

}   // namespace YTalk