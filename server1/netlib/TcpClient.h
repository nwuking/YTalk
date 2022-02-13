/**
 * @file TcpClient.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_TCP_CLIENT_H
#define YTALK_TCP_CLIENT_H

#include <string>
#include <mutex>
#include <memory>

#include "CallBacks.h"
#include "InetAddress.h"

namespace YTalk
{

namespace netlib
{

class EventLoop;
class Connector;

typedef std::shared_ptr<Connector> CONNECTOR_PTR;

class TcpClient
{
public:
    TcpClient(EventLoop *loop, const InetAddress &serverAddr, const std::string &name);
    ~TcpClient();

    void connect();

    void disconnect();

    void stop();

public:
    TcpConnectionPtr connection() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_connectionPtr;
    }

    EventLoop* getLoop() const {
        return m_loopPtr;
    }

    bool retry() const {
        return m_retry;
    }

    void enableRetry() {
        m_retry = true;
    }

    const std::string& name() const {
        return m_name;
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
    void newConnection(int fd);
    void removeConnection(const TcpConnectionPtr &conn);

private:
    EventLoop*                                      m_loopPtr;
    CONNECTOR_PTR                                   m_connectorPtr;
    const std::string                               m_name;
    ConnectionCallBack                              m_connectionCallBack;
    MessageCallback                                 m_messageCallBack;
    WriteCompleteCallback                           m_writeCompleteCallBack;
    bool                                            m_retry;
    bool                                            m_connect;
    int                                             m_nextConnId;
    mutable std::mutex                              m_mutex;
    TcpConnectionPtr                                m_connectionPtr;

};   // class TcpClient

    
}   /// namespce netlib

}   // namespace YTalk

#endif   /// YTALK_TCP_CLIENT_H