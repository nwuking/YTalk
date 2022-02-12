/**
 * @file Poller.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_EPOLLER_H
#define YTALK_EPOLLER_H

#include "../base/TimeStamp.h"

#include <vector>
#include <map>
#include <string>

#include <sys/epoll.h>

namespace YTalk
{

namespace netlib
{

class EventLoop;
class Channel;

class EPoller
{
public:
    typedef std::vector<Channel*>       CHANNEL_ARR;
    typedef std::vector<epoll_event>    EPOLL_EVENT_ARR;
    typedef std::map<int, Channel*>     CHANNEL_MAP;

public:
    EPoller(EventLoop *loop);
    ~EPoller();

    base::TimeStamp poll(CHANNEL_ARR *activeChannels, int timeout);

    void updateChannel(Channel *channel);

    void removeChannel(Channel *channel);

    bool hasChannel(Channel *channel) const;

    void assertInLoopThread() const;

private:
    void fillActiveChannels(CHANNEL_ARR *activeChannels, int numChannels);

    void update(int op, Channel *channel);

    std::string op2String(int op);

private:
    EventLoop*              m_loopPtr;
    int                     m_epollFd;
    EPOLL_EVENT_ARR         m_events;       // 用于保存epoll_wait()返回的事件
    CHANNEL_MAP             m_channels;

private:
    static const int        s_kInitEventArrSize = 16;

};  // class EPoller

}    //// namespace netlib

}   // namespace YTalk

#endif    /// YTALK_POLLER_H