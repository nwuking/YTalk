/**
 * @file EventLoopThreadPool.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"

#include <cstdio>

namespace YTalk
{

namespace netlib
{

EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop, int numThreads, const std::string &name) :
    m_baseLoopPtr(loop), 
    m_isStarted(false),
    m_numThreads(numThreads),
    m_next(0),
    m_name(name)
{
    //TODO
}

EventLoopThreadPool::~EventLoopThreadPool() {
    //TODO
}

void EventLoopThreadPool::start(const ThreadInitCallBack &cb) {
    if(m_baseLoopPtr == nullptr || m_isStarted) {
        return;
    }

    m_baseLoopPtr->assertInLoopThread();

    m_isStarted = true;
    for(int i = 0; i < m_numThreads; ++i) {
        char buf[m_name.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", m_name.c_str(), i);

        std::unique_ptr<EventLoopThread> e(new EventLoopThread(cb, buf));
        m_loops.push_back(e->startLoop());
        m_threads.push_back(std::move(e));
    }
    if(m_numThreads == 0 && cb) {
        cb(m_baseLoopPtr);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    m_baseLoopPtr->assertInLoopThread();
    if(!m_isStarted) {
        return nullptr;
    }

    EventLoop *loop = m_baseLoopPtr;

    if(!m_loops.empty()) {
        loop = m_loops[m_next];
        ++m_next;
        if(static_cast<std::size_t>(m_next) >= m_loops.size()) {
            m_next = 0;
        }
    }

    return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(std::size_t hashCode) {
    m_baseLoopPtr->assertInLoopThread();
    EventLoop *loop = m_baseLoopPtr;

    if(!m_loops.empty()) {
        loop = m_loops[hashCode % m_loops.size()];
    }

    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() {
    m_baseLoopPtr->assertInLoopThread();
    if(m_loops.empty()) {
        return std::vector<EventLoop*>(1, m_baseLoopPtr);
    }
    return m_loops;
}

} /// namepsce netlib

}   // namespace YTalk