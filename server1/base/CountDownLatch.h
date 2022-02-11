/**
 * @file CountDownLatch.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_COUNT_DOWN_LATCH_H
#define YTALK_COUNT_DOWN_LATCH_H

#include <mutex>
#include <condition_variable> 

namespace YTalk 
{

namespace base
{

class CountDownLatch
{
public:
    /**
     * @brief Construct a new Count Down Latch object
     * 
     * @param count 
     */
    explicit CountDownLatch(int count);
    /**
     * @brief 等待m_count为0
     * 
     */
    void wait();
    /**
     * @brief --m_count
     * 
     */
    void countDown();
    /**
     * @brief Get the Count object
     * 
     * @return int 
     */
    int getCount() const;

private:
    mutable std::mutex      m_mutex;
    std::condition_variable m_cond;
    int                     m_count;
};

}  // namespace base

}  // namespace YTalk

#endif    /// YTALK_COUNT_DOWN_LATCH_H