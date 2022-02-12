/**
 * @file Channel.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_CHANNEL_H
#define YTALK_CHANNEL_H

#include "CallBacks.h"

#include <string>

namespace YTalk
{

namespace netlib
{

class EventLoop;

class Channel
{
public:
    Channel(EventLoop *loop, int fd);
    ~Channel();

    void handleEvent(base::TimeStamp receiveTime);
    void remove();

    std::string revents2String();
    std::string events2String() {
        return events2String(m_fd, m_events);
    }

public:
    void setReadCallback(const ReadEventCallBack& cb) {
        m_readCallBack = cb;
    }
        
    void setWriteCallback(const EventCallBack& cb) {
        m_writeCallBack = cb;
    }

    void setCloseCallback(const EventCallBack& cb) {
        m_closeCallBack = cb;
    }

    void setErrorCallback(const EventCallBack& cb) {
    m_errorCallBack = cb;
    }

    int fd() {
        return m_fd;
    }

    int events() {
        return m_events;
    }

    void setRevents(int revents) {
        m_revents = revents;
    }

    void enableReading() {
        m_events |= s_kReadEvent;
        update();
    }
    void disableReading() {
        m_events &= ~s_kReadEvent;
        update();
    }

    void enableWriting() {
        m_events |= s_kWriteEvent;
        update();
    }
    void disableWriting() {
        m_events &= ~s_kWriteEvent;
        update();
    }

    void disableAll() {
        m_events = s_kNoneEvent;
        update();
    }

    bool isReading() {
        return m_events & s_kReadEvent;
    }
    bool isWriting() {
        return m_events & s_kWriteEvent;
    }
    bool isNoneEvent() {
        return m_events == s_kNoneEvent;
    }

    EventLoop* getOwnerLoop() {
        return m_loop;
    }

    int index() {
        return m_index;
    }

    void setIndex(int index) {
        m_index = index;
    }

private:
    std::string events2String(int fd, int events);
    void update();

private:
    ReadEventCallBack           m_readCallBack;
    EventCallBack               m_writeCallBack;
    EventCallBack               m_errorCallBack;
    EventCallBack               m_closeCallBack;

private:
    EventLoop*                  m_loop;
    const int                   m_fd;
    int                         m_events;       // Channel 所监听的事件
    int                         m_revents;      // 发生的事件
    bool                        m_handingEvent; // 是否在处理事件
    bool                        m_added2Loop;   // 是否加入到EventLoop里
    int                         m_index;

private:
    static const int            s_kNoneEvent;
    static const int            s_kReadEvent;
    static const int            s_kWriteEvent;

};   // class Channel


}    //// namespace netlib

}   // namespace YTalk

#endif    // YTALK_CHANNEL_H