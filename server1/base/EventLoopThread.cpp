/**
 * @file EventLoopThread.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "EventLoopThread.h"
#include "EventLoop.h"

namespace YTalk
{

namespace netlib
{

EventLoopThread::EventLoopThread(const ThreadInitCallBack &cb, const std::string &name) :
    m_loopPtr(nullptr),
    m_isExiting(false),
    m_callBack(cb)
{
    //TODO
}

EventLoopThread::~EventLoopThread() {
    m_isExiting = true;
    if(m_loopPtr != nullptr) {
        m_loopPtr->quit();
        m_threadPtr->join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    m_threadPtr.reset(new std::thread(std::bind(&EventLoopThread::threadFunc, this)));

    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_loopPtr == nullptr) {
            m_cond.wait(lock);
        }
        loop = m_loopPtr;
    }

    return loop;
}

void EventLoopThread::threadFunc() {
    // 在每个线程里生成EventLoop对象
    EventLoop loop;
    if(m_callBack) {
        m_callBack(&loop);
    }

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_loopPtr = &loop;
        m_cond.notify_all();
    }

    loop.loop();

    std::unique_lock<std::mutex> lock(m_mutex);
    m_loopPtr = nullptr;
}

}    // namespace netlib

}   // namespace YTalk