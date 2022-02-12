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

namespace YTalk
{

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

}   // namesapce netlib
 
}   // namespce YTalk