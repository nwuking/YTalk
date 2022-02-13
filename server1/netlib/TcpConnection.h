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

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop, const std::string &name, int sockfd, const InetAddress &localAddr, const InetAddress &peerAddr);
    ~TcpConnection();

    void send(const void *message, int len);
    void send(const std::string &message);

    void send(Buffer *buffer);

    void shutdown();

    void forceClose();

    void setTcpNoDelay(bool on);

    void connectEstablished();

    void connectDestroyed();

public:
    EventLoop* getLoop() const {
        return m_loopPtr;
    }

    const std::string& name() const {
        return m_name;
    }

    const InetAddress& localAddress() const {
        return m_localAddr;
    }

    const InetAddress& peerAddress() const {
        return m_peerAddr;
    }

    bool connected() {
        return m_state == kConnected;
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

    void setHighWaterMarkCallBack(const HighWaterMarkCallback &cb, std::size_t highWaterMark) {
        m_highWaterMarkCallBack = cb;
        m_highWaterMark = highWaterMark;
    }

    void setCloseCallBack(const CloseCallBack &cb) {
        m_closeCallBack = cb;
    }

    Buffer* inputBuffer() {
        return &m_inputBuffer;
    }

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
    void handleRead(base::TimeStamp receiveTime);

    void handleWrite();

    void handleClose();

    void handleError();

    void sendInLoop(const void *message, int len);

    void sendInLoop(const std::string &message);

    void shutdownInLoop();

    void forceCloseInLoop();

    std::string state2String();

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