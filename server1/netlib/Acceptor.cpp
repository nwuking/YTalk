/**
 * @file Acceptor.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"
#include "../base/Logger.h"

namespace YTalk
{

using namespace base;

namespace netlib
{

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort) : 
    m_loopPtr(loop), 
    m_acceptSocket(sockets::createNonblockingOrDie()),
    m_acceptChannel(loop, m_acceptSocket.fd()),
    m_listening(false)
{
    m_idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    m_acceptSocket.setReuseAddr(true);
    m_acceptSocket.setReusePort(reusePort);
    m_acceptSocket.bindAddress(listenAddr);
    m_acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    m_acceptChannel.disableAll();
    m_acceptChannel.remove();
    ::close(m_idleFd);
}

void Acceptor::listen() {
    m_loopPtr->assertInLoopThread();
    m_listening = true;
    m_acceptSocket.listen();
    m_acceptChannel.enableReading();
}

void Acceptor::handleRead() {
    m_loopPtr->assertInLoopThread();
    InetAddress peerAddr;
    int connFd = m_acceptSocket.accept(&peerAddr);
    if(connFd >= 0) {
        std::string ipPortStr = peerAddr.toIpPort();
        LOG_TRACE("Accept of %s", ipPortStr.c_str());

        if(m_newConnectionCallBack) {
            m_newConnectionCallBack(connFd, peerAddr);
        }
        else {
            sockets::close(connFd);
        }
    }
    else {
        LOG_SYSERROR("in Acceptor::handleRead");
        if(errno == EMFILE) {
            // 系统的fd已经用尽，快速处理到来的用户连接
            ::close(m_idleFd);
            m_idleFd = ::accept(m_acceptSocket.fd(), nullptr, nullptr);
            ::close(m_idleFd);
            m_idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}
    
}   /// namespce netlib

}   // namespace YTalk