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
#include "../base/Logger.h"

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

}   /// namespce netlib

}   // namespace YTalk