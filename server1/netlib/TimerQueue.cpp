/**
 * @file TimerQueue.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "TimerQueue.h"
#include "EventLoop.h"
#include "Timer.h"

namespace YTalk
{

using namespace base;

namespace netlib
{

TimerQueue::TimerQueue(EventLoop *loop) : m_loop(loop), m_timers() {
    //TODO
}

TimerQueue::~TimerQueue() {
    for(TIMER_LIST::iterator it = m_timers.begin(); it != m_timers.end(); ++it) {
        delete it->second;
    }
}

void TimerQueue::runTimer() {
    //TODO
}

TimerId TimerQueue::addTimer(const TimerCallBack &cb, base::TimeStamp when, std::int64_t interval, int64_t repeatCount) {
    Timer* timer = new Timer(cb, when, interval, repeatCount);
    m_loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

TimerId TimerQueue::addTimer(TimerCallBack &&cb, base::TimeStamp when, std::int64_t interval, int64_t repeatCount) {
    Timer* timer = new Timer(std::move(cb), when, interval, repeatCount);
    m_loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void TimerQueue::removeTimer(TimerId id) {
    m_loop->runInLoop(std::bind(&TimerQueue::removeTimerInLoop, this, id));
}

void TimerQueue::cancle(TimerId id, bool off) {
    m_loop->runInLoop(std::bind(&TimerQueue::cancelTimerInLoop, this, id, off));
}

void TimerQueue::addTimerInLoop(Timer *timer) {
    m_loop->assertInLoopThread();
}

void TimerQueue::removeTimerInLoop(TimerId id) {
    m_loop->assertInLoopThread();

    Timer* timer = id.m_timerPtr;
    for (auto iter = m_timers.begin(); iter != m_timers.end(); ++iter) {
        if (iter->second == timer) {
            m_timers.erase(iter);
            break;
        }
    }
}

void TimerQueue::cancelTimerInLoop(TimerId id, bool off) {
    m_loop->assertInLoopThread();

    Timer* timer = id.m_timerPtr;
    for (auto iter = m_timers.begin(); iter != m_timers.end(); ++iter)
    {
        if (iter->second == timer)
        {
            iter->second->cancle(off);
            break;
        }
    }
}

void TimerQueue::insert(Timer *timer) {
    //TODO
}

}   // namesapce netlib
 
}   // namespce YTalk