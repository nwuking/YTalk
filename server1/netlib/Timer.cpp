/**
 * @file Timer.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Timer.h"

namespace YTalk
{

namespace netlib
{

std::atomic<std::int64_t> Timer::s_numCreated;

Timer::Timer(const TimerCallBack &cb, base::TimeStamp when, std::int64_t interval, std::int64_t repeatConut/*= -1*/) : 
    m_callBack(cb), 
    m_expiration(when),
    m_interval(interval),
    m_repeatCount(repeatConut),
    m_isCancled(false),
    m_sequence(++s_numCreated)
{
    //TODO
}

Timer::Timer(TimerCallBack &&cb, base::TimeStamp when, std::int64_t interval) : 
    m_callBack(std::move(cb)), 
    m_expiration(when),
    m_interval(interval),
    m_repeatCount(-1),
    m_isCancled(false),
    m_sequence(++s_numCreated)
{
    //TODO
}

Timer::~Timer(){
    //TODO
}

void Timer::start() {
    if(m_isCancled) {
        return;
    }

    m_callBack();

    if(m_repeatCount != -1) {
        --m_repeatCount;
        if(m_repeatCount == 0) {
            return;
        }
    }

    m_expiration += m_interval;
}


}   // namespace netlib

}   // namesapce YTalk