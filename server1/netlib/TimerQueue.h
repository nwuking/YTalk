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

private:
    TimerQueue(const TimerQueue &obj) = delete;
    TimerQueue& operator=(const TimerQueue &obj) = delete;

private:
    typedef std::pair<base::TimeStamp, Timer*>      ENTRY;
    typedef std::set<ENTRY>                         TIMER_LIST;                    

private:
    EventLoop*      m_loop;
};    // class TimerQueue


}   // namesapce netlib
 
}   // namespce YTalk

#endif   // YTALK_TIMER_QUEUE_H