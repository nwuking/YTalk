/**
 * @file EventLoop.h
 * @author nwuking@qq.com
 * @brief ractor模式，one loop per thread
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_EVENT_LOOP_H
#define YTALK_EVENT_LOOP_H

#include "../base/TimeStamp.h"
#include "Types.h"
#include "TimerId.h"
#include "CallBacks.h"

#include <thread>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

namespace YTalk
{

namespace netlib
{

class Channel;
class EPoller;
class TimerQueue;

class EventLoop 
{
public:
    typedef std::function<void()> Functor;

public:
    /**
     * @brief one loop per thread，在每一个线程里创建一个EventLoop对象
     * 
     */
    EventLoop();
    /**
     * @brief Destroy the Event Loop object
     * 
     */
    ~EventLoop();
    /**
     * @brief 开始循环监听事件的发生，
     *        必须在同一线程里被调用
     */
    void loop();
    /**
     * @brief 停止循环监听事件
     * 
     */
    void quit();
    /**
     * @brief 如果在EventLoop所在的线程，直接调用cb，
     *        如果在其它线程，就将cb放入queue
     * 
     * @param cb 
     */
    void runInLoop(const Functor &cb);
    /**
     * @brief 将cb放入EventLoop所在线程的queue
     * 
     * @param cb 
     */
    void queueInLoop(const Functor &cb);
    /**
     * @brief 定时器在什么时候跑
     * 
     * @param when 
     * @param cb 
     * @return TimerId 
     */
    TimerId runAt(const base::TimeStamp &when, const TimerCallBack &cb);
    /**
     * @brief 定时器在多少时间后跑
     * 
     * @param delay 
     * @param cb 
     * @return TimerId 
     */
    TimerId runAfter(std::int64_t delay, const TimerCallBack &cb);
    /**
     * @brief 循环定时器
     * 
     * @param interval 
     * @param cb 
     * @return TimerId 
     */
    TimerId runEvery(std::int64_t interval, const TimerCallBack &cb);
    /**
     * @brief 根据定时器id，取消定时器
     * 
     * @param id 
     * @param off 
     */
    void cancle(TimerId id, bool off);
    /**
     * @brief 将定时器从set移除
     * 
     * @param id 
     */
    void remove(TimerId id);
    /**
     * @brief 会调用epoller.updateChannel()，更新channel
     * 
     * @param channel 
     */
    void updateChannel(Channel *channel);
    /**
     * @brief 移除channel
     * 
     * @param channel 
     */
    void removeChannel(Channel *channel);
    /**
     * @brief 
     * 
     * @param channel 
     * @return true 
     * @return false 
     */
    bool hasChannel(Channel *channel);

public:
    /**
     * @brief 通过这个判断当前操作是否在正确的线程
     * 
     */
    void assertInLoopThread() {
        if(!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }
    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool isInLoopThread() {
        return m_threadId == std::this_thread::get_id();
    }
    /**
     * @brief epoll的返回时间
     * 
     * @return base::TimeStamp 
     */
    base::TimeStamp pollReturnTime() {
        return m_pollReturnTime;
    }
    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool eventHanding() {
        return m_eventHanding;
    }

private:
    /**
     * @brief 用于唤醒当前线程时的回调函数
     * 
     */
    void handleRead();     // for wakeupFd;
    /**
     * @brief 以字符串显示活跃的Channel
     * 
     */
    void printActiveChannels();
    /**
     * @brief 允许队列中的任务
     * 
     */
    void doPendingFunctors();
    /**
     * @brief 
     * 
     */
    void wakeup();
    /**
     * @brief 操作不在它所在的线程，会打印日志，然后退出
     * 
     */
    void abortNotInLoopThread();

private:
    typedef std::vector<Channel*> CHANNEL_ARR;

private:
    bool                        m_isLooping;            
    std::atomic<bool>           m_isQuited;
    bool                        m_eventHanding;
    const std::thread::id       m_threadId;
    base::TimeStamp             m_pollReturnTime;
    std::unique_ptr<EPoller>    m_pollerPtr;
    std::unique_ptr<TimerQueue> m_timerQueuePtr;
    SOCKET                      m_wakeupFd;
    std::unique_ptr<Channel>    m_wakeupChannelPtr;
    CHANNEL_ARR                 m_activeChannels;
    Channel*                    m_curActiveChannelPtr;
    std::mutex                  m_mutex;
    bool                        m_callingPendingFunctors;
    std::vector<Functor>        m_pendingFunctors;

};   // class EventLoop

}   // namespace netlib

}   // namespce YTalk

#endif  // YTALK_EVENT_LOOP_H