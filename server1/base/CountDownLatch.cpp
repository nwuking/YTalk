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

#include "CountDownLatch.h"

namespace YTalk
{

namespace base
{

CountDownLatch::CountDownLatch(int count)
    : m_count(count)
{
}

void CountDownLatch::wait()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_count > 0)
    {
        m_cond.wait(lock);
    }
}

void CountDownLatch::countDown()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    --m_count;
    if (m_count == 0)
    {
        m_cond.notify_all();
    }
}

int CountDownLatch::getCount() const
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_count;
}

}   // namespae YTalk

}   // namespace YTalk