/**
 * @file EventLoopThread.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_EVENT_LOOP_THREAD_H
#define YTALK_EVENT_LOOP_THREAD_H

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <string>

namespace YTalk
{

namespace netlib
{

class EventLoop;

class EventLoopThread
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallBack;

public:
    EventLoopThread(const ThreadInitCallBack &cb = ThreadInitCallBack(), const std::string &name = " ");
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

private:
    EventLoop*                      m_loopPtr;
    std::unique_ptr<std::thread>    m_threadPtr;
    std::mutex                      m_mutex;
    std::condition_variable         m_cond;
    bool                            m_isExiting;

private:
    ThreadInitCallBack              m_callBack;

};   // class EventLoopThread
    
}    // namespace netlib

}   // namespace YTalk

#endif   // YTALK_EVENT_LOOP_THREAD_H
