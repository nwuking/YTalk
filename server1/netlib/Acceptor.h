/**
 * @file Acceptor.h
 * @author nwuking@qq.com
 * @brief 由TcpSever使用，accept()的抽象，等待接收客户端的连接。
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_ACCEPTOR_H
#define YTALK_ACCEPTOR_H

#include <functional>

#include "Socket.h"
#include "Channel.h"

namespace YTalk
{

namespace netlib
{

class InetAddress;
class EventLoop;

class Acceptor
{
public:
    typedef std::function<void(int fd, const InetAddress&)> NewConnectionCallBack;

public:
    /**
     * @brief Construct a new Acceptor object
     * 
     * @param loop 线程中的EventLoop，
     * @param listenAddr 服务端所监听的ip地址
     * @param reusePort port是否要重用
     */
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort);
    ~Acceptor();
    /**
     * @brief 相当于socket.listen()
     * 
     */
    void listen();

public:
    /**
     * @brief 设置每个新连接的回调函数，当有新的连接到来，
     *        会调用这个回调函数生成一个Connnection对象
     *         
     * @param cb 
     */
    void setNewConnectionCallBack(const NewConnectionCallBack &cb) {
        m_newConnectionCallBack = cb;
    }

    bool listening() const {
        return m_listening;
    }

private:
    /**
     * @brief Channel的回调函数，Channel相应的事件可读的时候，
     *          调用这个回调函数
     */
    void handleRead();

private:
    EventLoop*                          m_loopPtr;                  
    Socket                              m_acceptSocket;
    Channel                             m_acceptChannel;
    NewConnectionCallBack               m_newConnectionCallBack;
    bool                                m_listening;
    int                                 m_idleFd;                   // 在系统的fd用尽的时候使用这个描述符

};   /// class Acceptor

    
}   /// namespce netlib

}   // namespace YTalk

#endif  // YTALK_ACCEPTOR_H