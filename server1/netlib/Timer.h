/**
 * @file Timer.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_TIMER_H
#define YTALK_TIMER_H

#include "../base/TimeStamp.h"
#include "CallBacks.h"

#include <atomic>
#include <cstdint>

namespace YTalk
{

namespace netlib
{

class Timer
{
public:
    /**
     * @brief Construct a new Timer object
     * 
     * @param cb 定时器的回调函数
     * @param when 定时时间
     * @param interval 单位是微秒
     * @param repeatConut 定时器重复次数
     */
    Timer(const TimerCallBack &cb, base::TimeStamp when, std::int64_t interval, std::int64_t repeatConut = -1);
    Timer(TimerCallBack &&cb, base::TimeStamp when, std::int64_t interval);
    ~Timer();
    /**
     * @brief 开始定时器  
     * 
     */
    void start();
    /**
     * @brief 取消定时器
     * 
     */
    void cancle(bool off) {
        m_isCancled = off;
    }

    base::TimeStamp expiration() const { return m_expiration; }
    int64_t getRepeatCount() const { return m_repeatCount; }
    int64_t sequence() const { return m_sequence; }
    static int64_t numCreated() { return s_numCreated; }

private:
    Timer(const Timer &timer) = delete;
    Timer& operator=(const Timer &timer) = delete;

private:
    TimerCallBack       m_callBack;
    base::TimeStamp     m_expiration;
    const std::int64_t  m_interval;
    std::int64_t        m_repeatCount;   // -1表示一直循环下去
    bool                m_isCancled;
    const std::int64_t  m_sequence;

private:
    static std::atomic<std::int64_t>    s_numCreated;
};   // class Timer

}   // namespace netlib

}   // namesapce YTalk

#endif   // YTALK_TIMER_H