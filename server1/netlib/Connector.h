/**
 * @file Connector.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_CONNECTOR_H
#define YTALK_CONNECTOR_H

#include <functional>
#include <memory>

#include "InetAddress.h"

namespace YTalk
{

namespace netlib
{

class Channel;
class EventLoop;

class Connector : public std::enable_shared_from_this<Connector>
{
public:
    typedef std::function<void(int sockfd)> NewConnectionCallback;

public:
    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();

    void start();
    void restart();
    void stop();

public:
    void setNewConnectionCallback(const NewConnectionCallback& cb) {
            m_newConnectionCallBack = cb;
    }

    const InetAddress& serverAddress() const { 
        return m_serverAddr; 
    }

private:
    enum State { 
        kDisconnected, 
        kConnecting, 
        kConnected 
    };
    static const int kMaxRetryDelayMs = 30 * 1000;
    static const int kInitRetryDelayMs = 500;

    void setState(State s) { m_state = s; }
    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

private:
    EventLoop*                          m_loopPtr;
    InetAddress                         m_serverAddr;
    bool                                m_connect;
    State                               m_state;
    std::unique_ptr<Channel>            m_channelPtr;
    NewConnectionCallback               m_newConnectionCallBack;
    int                                 m_retryDelayMs;

};     /// class Connector
    
}   /// namespce netlib

}   // namespace YTalk

#endif  // YTALK_CONNECTOR_H