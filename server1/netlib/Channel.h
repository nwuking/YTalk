/**
 * @file Channel.h
 * @author nwuking@qq.com
 * @brief Channel，Reactor模式的事件的抽象
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
    /**
     * @brief Construct a new Channel object
     * 
     * @param loop 所在线程的loop
     * @param fd 被监听的描述符，一个Channel一个fd
     */
    Channel(EventLoop *loop, int fd);
    /**
     * @brief Destroy the Channel object
     * 
     */
    ~Channel();
    /**
     * @brief Channel有事件发生的时候的回调函数。
     * 
     * @param receiveTime 事件发生的时间
     */
    void handleEvent(base::TimeStamp receiveTime);
    /**
     * @brief 会调用EventLoop.removeChannel()，从监听中移除
     * 
     */
    void remove();
    /**
     * @brief 事件字符串化
     * 
     * @return std::string 
     */
    std::string revents2String();

public: 
    /**
     * @brief 
     * 
     * @return std::string 
     */
    std::string events2String() {
        return events2String(m_fd, m_events);
    }
    /**
     * @brief 设置发生可读事件时的回调函数
     * 
     * @param cb 
     */
    void setReadCallback(const ReadEventCallBack& cb) {
        m_readCallBack = cb;
    }
    /**
     * @brief 设置发生可写事件时的回调函数
     * 
     * @param cb 
     */
    void setWriteCallback(const EventCallBack& cb) {
        m_writeCallBack = cb;
    }
    /**
     * @brief 设置关闭的回调函数
     * 
     * @param cb 
     */
    void setCloseCallback(const EventCallBack& cb) {
        m_closeCallBack = cb;
    }
    /**
     * @brief 设置发生错误时的回调函数
     * 
     * @param cb 
     */
    void setErrorCallback(const EventCallBack& cb) {
        m_errorCallBack = cb;
    }
    /**
     * @brief 返回Channel拥有的描述符
     * 
     * @return int 
     */
    int fd() {
        return m_fd;
    }
    /**
     * @brief 返回监听的事件
     * 
     * @return int 
     */
    int events() {
        return m_events;
    }
    /**
     * @brief 设置发生的事件
     * 
     * @param revents 
     */
    void setRevents(int revents) {
        m_revents = revents;
    }
    /**
     * @brief 设置可读事件
     * 
     */
    void enableReading() {
        m_events |= s_kReadEvent;
        update();
    }
    /**
     * @brief 取消可读事件
     * 
     */
    void disableReading() {
        m_events &= ~s_kReadEvent;
        update();
    }
    /**
     * @brief 设置可写数据
     * 
     */
    void enableWriting() {
        m_events |= s_kWriteEvent;
        update();
    }
    /**
     * @brief 取消可写数据
     * 
     */
    void disableWriting() {
        m_events &= ~s_kWriteEvent;
        update();
    }
    /**
     * @brief 取消所有数据
     * 
     */
    void disableAll() {
        m_events = s_kNoneEvent;
        update();
    }
    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool isReading() {
        return m_events & s_kReadEvent;
    }
    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool isWriting() {
        return m_events & s_kWriteEvent;
    }
    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool isNoneEvent() {
        return m_events == s_kNoneEvent;
    }
    /**
     * @brief Get the Owner Loop object
     * 
     * @return EventLoop* 
     */
    EventLoop* getOwnerLoop() {
        return m_loop;
    }
    /**
     * @brief Channel在EventLoop中的小标位置
     * 
     * @return int 
     */
    int index() {
        return m_index;
    }
    /**
     * @brief Set the Index object
     * 
     * @param index 
     */
    void setIndex(int index) {
        m_index = index;
    }

private:
    std::string events2String(int fd, int events);
    /**
     * @brief 会调用EventLoop.updateChannel()，更新事件
     * 
     */
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
    int                         m_index;        // 在EventLoop中的小标位置

private:
    static const int            s_kNoneEvent;
    static const int            s_kReadEvent;
    static const int            s_kWriteEvent;

};   // class Channel


}    //// namespace netlib

}   // namespace YTalk

#endif    // YTALK_CHANNEL_H