/**
 * @file Connector.h
 * @author nwuking@qq.com
 * @brief 由TcpCliennt使用，连接者，用于连接服务端
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

/**
 * @brief 继承enable_shared_from_this<>
 *        
 */
class Connector : public std::enable_shared_from_this<Connector>
{
public:
    typedef std::function<void(int sockfd)> NewConnectionCallback;

public:
    /**
     * @brief Construct a new Connector object
     * 
     * @param loop 所在线程的loop
     * @param serverAddr 服务端的地址
     */
    Connector(EventLoop* loop, const InetAddress& serverAddr);
    /**
     * @brief Destroy the Connector object
     * 
     */
    ~Connector();
    /**
     * @brief 开始连接服务端，会调用startInLoop()
     * 
     */
    void start();
    /**
     * @brief 重新连接
     * 
     */
    void restart();
    /**
     * @brief 停止，会调用stopInLoop()
     * 
     */
    void stop();

public:
    /**
     * @brief 设置连接成功时的回调函数
     * 
     * @param cb 
     */
    void setNewConnectionCallback(const NewConnectionCallback& cb) {
            m_newConnectionCallBack = cb;
    }
    /**
     * @brief 返回服务端的地址
     * 
     * @return const InetAddress& 
     */
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
    /**
     * @brief 设置连接的状态
     * 
     * @param s 
     */
    void setState(State s) { 
        m_state = s;
    }
    /**
     * @brief 确保在loop所在的线程允许
     * 
     */
    void startInLoop();
    /**
     * @brief 确保在loop所在的线程停止
     * 
     */
    void stopInLoop();
    /**
     * @brief 调用socket.connect()，连接服务端
     * 
     */
    void connect();
    /**
     * @brief 连接中，此时会设置相应的channel的参数
     * 
     * @param sockfd 
     */
    void connecting(int sockfd);
    /**
     * @brief 
     * 
     */
    void handleWrite();
    /**
     * @brief 发生错误时会调用这个函数
     * 
     */
    void handleError();
    /**
     * @brief 尝试重新连接
     * 
     * @param sockfd 
     */
    void retry(int sockfd);
    /**
     * @brief 移除原本的channel，重置channel
     * 
     * @return int 
     */
    int removeAndResetChannel();
    /**
     * @brief 重置channel
     * 
     */
    void resetChannel();

private:
    EventLoop*                          m_loopPtr;
    InetAddress                         m_serverAddr;   // 服务端Channel
    bool                                m_connect;      // 是否连接
    State                               m_state;        // 连接状态
    std::unique_ptr<Channel>            m_channelPtr;   // 
    NewConnectionCallback               m_newConnectionCallBack;   // 连接成功时的回调函数
    int                                 m_retryDelayMs;            // 连接时的等待时间，超过重连

};     /// class Connector
    
}   /// namespce netlib

}   // namespace YTalk

#endif  // YTALK_CONNECTOR_H