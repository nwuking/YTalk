/**
 * @file EventLoopThreadPool.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef YTALK_EVENT_LOOP_THREAD_POOL_H
#define YTALK_EVENT_LOOP_THREAD_POOL_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cstdint>

namespace YTalk
{

namespace netlib
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallBack;

public:
    EventLoopThreadPool(EventLoop *loop, int numThreads, const std::string &name);
    ~EventLoopThreadPool();

    void start(const ThreadInitCallBack &cb = ThreadInitCallBack());

    EventLoop* getNextLoop();

    EventLoop* getLoopForHash(std::size_t hashCode);

    std::vector<EventLoop*> getAllLoops();

public:
    bool started() const {
        return m_isStarted;
    }

    const std::string& name() const {
        return m_name;
    }

private:
    EventLoop*                                      m_baseLoopPtr;
    std::string                                     m_name;
    bool                                            m_isStarted;
    int                                             m_numThreads;
    int                                             m_next;
    std::vector<std::unique_ptr<EventLoopThread>>   m_threads;
    std::vector<EventLoop*>                         m_loops;

};   // class EventLoopThreadPool

    
} /// namepsce netlib

}   // namespace YTalk

#endif  ///YTALK_EVENT_LOOP_THREAD_POOL_H