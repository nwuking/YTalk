/**
 * @file Acceptor.h
 * @author nwuking@qq.com
 * @brief 
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
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort);
    ~Acceptor();

    void listen();

public:
    void setNewConnectionCallBack(const NewConnectionCallBack &cb) {
        m_newConnectionCallBack = cb;
    }

    bool listening() const {
        return m_listening;
    }

private:
    void handleRead();

private:
    EventLoop*                          m_loopPtr;
    Socket                              m_acceptSocket;
    Channel                             m_acceptChannel;
    NewConnectionCallBack               m_newConnectionCallBack;
    bool                                m_listening;
    int                                 m_idleFd;

};   /// class Acceptor

    
}   /// namespce netlib

}   // namespace YTalk

#endif  // YTALK_ACCEPTOR_H