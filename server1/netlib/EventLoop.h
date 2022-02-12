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

    void runInLoop(const Functor &cb);

    void queueInLoop(const Functor &cb);

    TimerId runAt(const base::TimeStamp &when, const TimerCallBack &cb);

    TimerId runAfter(std::int64_t delay, const TimerCallBack &cb);

    TimerId runEvery(std::int64_t interval, const TimerCallBack &cb);

    void cancle(TimerId id, bool off);

    void remove(TimerId id);

    void updateChannel(Channel *channel);

    void removeChannel(Channel *channel);

    bool hasChannel(Channel *channel);

public:
    void assertInLoopThread() {
        if(!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() {
        return m_threadId == std::this_thread::get_id();
    }

    base::TimeStamp pollReturnTime() {
        return m_pollReturnTime;
    }

    bool eventHanding() {
        return m_eventHanding;
    }

private:
    void handleRead();     // for wakeupFd;

    void printActiveChannels();

    void doPendingFunctors();

    void wakeup();

    void abortNotInLoopThread();

private:
    typedef std::vector<Channel*> CHANNEL_ARR;

private:
    bool                        m_isLooping;
    //bool                        m_isQuited;
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