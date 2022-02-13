/**
 * @file TcpServer.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_TCP_SERVER_H
#define YTALK_TCP_SERVER_H

#include <string>
#include <memory>
#include <atomic>
#include <map>

#include "CallBacks.h"
#include "InetAddress.h"

namespace YTalk
{

namespace netlib
{

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallBack;
    enum Option {
        kNoReusePort,
        kReusePort
    };

public:
    TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &name, Option option = kReusePort);
    ~TcpServer();

    void start(int wokers = 4);

    //void stop();

    void removeConnection(const TcpConnectionPtr &conn);

public:
    const std::string& ipPort() {
        return m_ipPort;
    }

    const std::string& name() {
        return m_name;
    }

    EventLoop* getLoop() {
        return m_loopPtr;
    }

    void setThreadInitCallBack(const ThreadInitCallBack &cb) {
        m_threadInitCalBack = cb;
    }

    void setConnectionCallBack(const ConnectionCallBack &cb) {
        m_connectionCallBack = cb;
    }

    void setMessageCallBack(const MessageCallback &cb) {
        m_messageCallBack = cb;
    }

    void setWriteCompleteCallBack(const WriteCompleteCallback &cb) {
        m_writeCompleteCallBack = cb;
    }

private:
    void newConnection(int sockFd, const InetAddress &peerAddr);

    void removeConnectionInLoop(const TcpConnectionPtr &conn);

private:
    typedef std::map<std::string, TcpConnectionPtr> CONNECTION_MAP;

private:
    EventLoop*                                  m_loopPtr;
    const std::string                           m_ipPort;
    const std::string                           m_name;
    std::unique_ptr<Acceptor>                   m_acceptor;
    std::unique_ptr<EventLoopThreadPool>        m_eventLoopThreadPool;
    std::atomic<int>                            m_started;
    int                                         m_nextConnId;
    CONNECTION_MAP                              m_connections;

private:
    ConnectionCallBack                          m_connectionCallBack;
    MessageCallback                             m_messageCallBack;
    WriteCompleteCallback                       m_writeCompleteCallBack;
    ThreadInitCallBack                          m_threadInitCalBack;

};   /// class TcpServer
    
}   /// namespce netlib

}   // namespace YTalk

#endif   /// YTALK_TCP_SERVER_H