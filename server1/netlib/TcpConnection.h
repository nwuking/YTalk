/**
 * @file TcpConnection.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_TCP_CONNECTION_H
#define YTALK_TCP_CONNECTION_H

#include <string>
#include <memory>
#include <cstdint>

#include "InetAddress.h"
#include "CallBacks.h"
#include "Buffer.h"
#include "../base/TimeStamp.h"

namespace YTalk
{

namespace netlib
{

class Channel;
class Socket;
class EventLoop;
/**
 * @brief 继承enable_shared_from_this<>
 * 
 */
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    /**
     * @brief Construct a new Tcp Connection object
     * 
     * @param loop 
     * @param name 
     * @param sockfd 
     * @param localAddr 
     * @param peerAddr 
     */
    TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr);
    /**
     * @brief Destroy the Tcp Connection object
     * 
     */
    ~TcpConnection();
    /**
     * @brief 
     * 
     * @param message 
     * @param len 
     */
    void send(const void *message, int len);
    /**
     * @brief 
     * 
     * @param message 
     */
    void send(const std::string &message);
    /**
     * @brief 
     * 
     * @param buffer 
     */
    void send(Buffer *buffer);
    /**
     * @brief 半关闭
     * 
     */
    void shutdown();
    /**
     * @brief 强制关闭
     * 
     */
    void forceClose();
    /**
     * @brief Set the Tcp No Delay object
     * 
     * @param on 
     */
    void setTcpNoDelay(bool on);
    /**
     * @brief 
     * 
     */
    void connectEstablished();
    /**
     * @brief 
     * 
     */
    void connectDestroyed();

public:
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
     * @return const std::string& 
     */
    const std::string& name() const {
        return m_name;
    }
    /**
     * @brief 
     * 
     * @return const InetAddress& 
     */
    const InetAddress& localAddress() const {
        return m_localAddr;
    }
    /**
     * @brief 
     * 
     * @return const InetAddress& 
     */
    const InetAddress& peerAddress() const {
        return m_peerAddr;
    }
    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool connected() {
        return m_state == kConnected;
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
    /**
     * @brief Set the High Water Mark Call Back object
     * 
     * @param cb 
     * @param highWaterMark 
     */
    void setHighWaterMarkCallBack(const HighWaterMarkCallback &cb, std::size_t highWaterMark) {
        m_highWaterMarkCallBack = cb;
        m_highWaterMark = highWaterMark;
    }
    /**
     * @brief Set the Close Call Back object
     * 
     * @param cb 
     */
    void setCloseCallBack(const CloseCallBack &cb) {
        m_closeCallBack = cb;
    }
    /**
     * @brief 
     * 
     * @return Buffer* 
     */
    Buffer* inputBuffer() {
        return &m_inputBuffer;
    }
    /**
     * @brief 
     * 
     * @return Buffer* 
     */
    Buffer* outputBuffer() {
        return &m_outputBuffer;
    }

private:
    enum StateE {
        kDisconnected, 
        kConnecting, 
        kConnected, 
        kDisconnecting
    };

private:
    /**
     * @brief 
     * 
     * @param receiveTime 
     */
    void handleRead(base::TimeStamp receiveTime);
    /**
     * @brief 
     * 
     */
    void handleWrite();
    /**
     * @brief 
     * 
     */
    void handleClose();
    /**
     * @brief 
     * 
     */
    void handleError();
    /**
     * @brief 
     * 
     * @param message 
     * @param len 
     */
    void sendInLoop(const void *message, int len);
    /**
     * @brief 
     * 
     * @param message 
     */
    void sendInLoop(const std::string &message);
    /**
     * @brief 
     * 
     */
    void shutdownInLoop();
    /**
     * @brief 
     * 
     */
    void forceCloseInLoop();
    /**
     * @brief 
     * 
     * @return std::string 
     */
    std::string state2String();
    /**
     * @brief Set the State object
     * 
     * @param s 
     */
    void setState(StateE s) {
        m_state = s;
    }

private:
    EventLoop*                                      m_loopPtr;
    const std::string                               m_name;
    StateE                                          m_state;
    std::unique_ptr<Socket>                         m_socketPtr;
    std::unique_ptr<Channel>                        m_channelPtr;
    const InetAddress                               m_localAddr;
    const InetAddress                               m_peerAddr;
    std::size_t                                     m_highWaterMark;
    Buffer                                          m_inputBuffer;
    Buffer                                          m_outputBuffer;

private:
    ConnectionCallBack                              m_connectionCallBack;
    MessageCallback                                 m_messageCallBack;
    WriteCompleteCallback                           m_writeCompleteCallBack;
    HighWaterMarkCallback                           m_highWaterMarkCallBack;
    CloseCallBack                                   m_closeCallBack;

};    // TcpConnection
    
}   /// namespce netlib

}   // namespace YTalk

#endif   /// YTALK_TCP_CONNECTION_H