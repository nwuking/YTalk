/**
 * @file Poller.h
 * @author nwuking@qq.com
 * @brief epoll()的封装，reactor模式中的监听器的抽象
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
    /**
     * @brief Construct a new EPoller object
     * 
     * @param loop 
     */
    EPoller(EventLoop *loop);
    /**
     * @brief Destroy the EPoller object
     * 
     */
    ~EPoller(); 
    /**
     * @brief 调用epoll_wait()，会阻塞，发生事件时，会将活跃的channel返回
     * 
     * @param activeChannels 返回活跃的channel
     * @param timeout 
     * @return base::TimeStamp 
     */
    base::TimeStamp poll(CHANNEL_ARR *activeChannels, int timeout);
    /**
     * @brief 更新channel，epoll_ctl()
     * 
     * @param channel 
     */
    void updateChannel(Channel *channel);
    /**
     * @brief 移除相应的channel.fd()
     * 
     * @param channel 
     */
    void removeChannel(Channel *channel);
    /**
     * @brief 是否存在这个channel
     * 
     * @param channel 
     * @return true 
     * @return false 
     */
    bool hasChannel(Channel *channel) const;
    /**
     * @brief 
     * 
     */
    void assertInLoopThread() const;

private:
    /**
     * @brief 在poll()中调用，将活跃的channel填充到数组里
     * 
     * @param activeChannels 
     * @param numChannels 
     */
    void fillActiveChannels(CHANNEL_ARR *activeChannels, int numChannels);
    /**
     * @brief 在updateChannel中调用，根据op执行相应的操作
     * 
     * @param op 
     * @param channel 
     */
    void update(int op, Channel *channel);
    /**
     * @brief 
     * 
     * @param op 
     * @return std::string 
     */
    std::string op2String(int op);

private:
    EventLoop*              m_loopPtr;
    int                     m_epollFd;      // epoll的句柄
    EPOLL_EVENT_ARR         m_events;       // 用于保存epoll_wait()返回的事件
    CHANNEL_MAP             m_channels;

private:
    static const int        s_kInitEventArrSize = 16;

};  // class EPoller

}    //// namespace netlib

}   // namespace YTalk

#endif    /// YTALK_POLLER_H