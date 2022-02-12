/**
 * @file TimerQueue.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_TIMER_QUEUE_H
#define YTALK_TIMER_QUEUE_H

#include "../base/TimeStamp.h"

#include "CallBacks.h"
#include "TimerId.h"

#include <set>      // 利用set的底层红黑树管理Timer

namespace YTalk
{

namespace netlib
{

class Timer;
class EventLoop;

class TimerQueue
{
public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();
    /**
     * @brief 时间到的时候，允许timer
     * 
     */
    void runTimer();

    TimerId addTimer(const TimerCallBack &cb, base::TimeStamp when, std::int64_t interval, int64_t repeatCount);
    TimerId addTimer(TimerCallBack &&cb, base::TimeStamp when, std::int64_t interval, int64_t repeatCount);

    void removeTimer(TimerId id);

    void cancle(TimerId id, bool off);

private:
    TimerQueue(const TimerQueue &obj) = delete;
    TimerQueue& operator=(const TimerQueue &obj) = delete;

    void addTimerInLoop(Timer *timer);

    void removeTimerInLoop(TimerId id);

    void cancelTimerInLoop(TimerId id, bool off);

    void insert(Timer *timer);

private:
    typedef std::pair<base::TimeStamp, Timer*>      ENTRY;
    typedef std::set<ENTRY>                         TIMER_LIST;                    

private:
    EventLoop*      m_loop;
    TIMER_LIST      m_timers;
};    // class TimerQueue


}   // namesapce netlib
 
}   // namespce YTalk

#endif   // YTALK_TIMER_QUEUE_H