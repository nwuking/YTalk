/**
 * @file TcpClient.h
 * @author nwuking@qq.com
 * @brief 客户端
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
    /**
     * @brief Construct a new Tcp Client object
     * 
     * @param loop 
     * @param serverAddr 
     * @param name 
     */
    TcpClient(EventLoop *loop, const InetAddress &serverAddr, const std::string &name);
    /**
     * @brief Destroy the Tcp Client object
     * 
     */
    ~TcpClient();
    /**
     * @brief 连接服务端
     * 
     */
    void connect();
    /**
     * @brief 关闭连接
     * 
     */
    void disconnect();
    /**
     * @brief 
     * 
     */
    void stop();

public:
    /**
     * @brief 
     * 
     * @return TcpConnectionPtr 
     */
    TcpConnectionPtr connection() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_connectionPtr;
    }
    /**
     * @brief Get the Loop object
     * 
     * @return EventLoop* 
     */
    EventLoop* getLoop() const {
        return m_loopPtr;
    }
    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool retry() const {
        return m_retry;
    }
    /**
     * @brief 
     * 
     */
    void enableRetry() {
        m_retry = true;
    }
    /**
     * @brief 
     * 
     * @return const std::string& 
     */
    const std::string& name() const {
        return m_name;
    }
    /**
     * @brief Set the Connection Call Back object
     * 
     * @param cb 
     */
    void setConnectionCallBack(const ConnectionCallBack &cb) {
        m_connectionCallBack = cb;
    }
    /**
     * @brief Set the Message Call Back object
     * 
     * @param cb 
     */
    void setMessageCallBack(const MessageCallback &cb) {
        m_messageCallBack = cb;
    }
    /**
     * @brief Set the Write Complete Call Back object
     * 
     * @param cb 
     */
    void setWriteCompleteCallBack(const WriteCompleteCallback &cb) {
        m_writeCompleteCallBack = cb;
    }

private:
    /**
     * @brief 连接成功时，生成一个Connection对象
     * 
     * @param fd 
     */
    void newConnection(int fd);
    /**
     * @brief 
     * 
     * @param conn 
     */
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