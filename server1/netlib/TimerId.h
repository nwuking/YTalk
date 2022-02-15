/**
 * @file TimerId.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_TIMER_ID_H
#define YTALK_TIMER_ID_H

#include <cstdint>

namespace YTalk 
{

namespace netlib
{

class Timer;

class TimerId 
{
public:
    /**
     * @brief Construct a new Timer Id object
     * 
     */
    TimerId() : m_timerPtr(nullptr), m_sequence(0) {
        //TODO
    }
    /**
     * @brief Construct a new Timer Id object
     * 
     * @param timer 
     * @param sequence 
     */
    TimerId(Timer *timer, std::int64_t sequence) :
        m_timerPtr(timer),
        m_sequence(sequence)
    {
        //TODO
    }
    /**
     * @brief Get the Timer object
     * 
     * @return Timer* 
     */
    Timer* getTimer() {
        return m_timerPtr;
    }
    /**
     * @brief Get the Sequence object
     * 
     * @return std::int64_t 
     */
    std::int64_t getSequence() {
        return m_sequence;
    }

    friend class TimerQueue;

private:
    Timer*              m_timerPtr;
    std::int64_t        m_sequence;
};   // class TimerId

}  // namespace netlib

}   // namespace YTalk


#endif  //YTALK_TIMER_ID_H