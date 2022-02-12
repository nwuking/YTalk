/**
 * @file EventLoop.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "EventLoop.h"
#include "TimerQueue.h"
#include "EPoller.h"
#include "Channel.h"
#include "../base/Logger.h"
#include "SocketsOps.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <signal.h>
#include <sstream>

namespace YTalk
{

namespace netlib
{

using namespace base;

const int kPollTimeMs = 10000;

__thread EventLoop*     t_loopInThisThread = nullptr;

static int createEventFd() {
    int eveFd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if(eveFd < 0) {
        LOG_FATAL("Fail to create eventFd");
    }
    return eveFd;
}

EventLoop::EventLoop() : 
    m_isLooping(false), 
    m_isQuited(false), 
    m_eventHanding(false), 
    m_threadId(std::this_thread::get_id()),
    m_pollerPtr(new EPoller(this)),
    m_timerQueuePtr(new TimerQueue(this)),
    m_wakeupFd(createEventFd()),
    m_wakeupChannelPtr(new Channel(this, m_wakeupFd)),
    m_curActiveChannelPtr(nullptr),
    m_mutex(),
    m_callingPendingFunctors(false)
{
    LOG_DEBUG("EventLoop created in thread=%d", m_threadId);
    if(t_loopInThisThread) {
        LOG_FATAL("Another EventLoop exists in this thread=%d", m_threadId);
    }
    else {
        t_loopInThisThread = this;
    }

    m_wakeupChannelPtr->setReadCallback(std::bind(&EventLoop::handleRead, this));
    m_wakeupChannelPtr->enableReading();
}

EventLoop::~EventLoop() {
    LOG_DEBUG("EventLoop of thread=%d, destruct in thread=%d", m_threadId, std::this_thread::get_id());
    m_wakeupChannelPtr->disableAll();
    m_wakeupChannelPtr->remove();
    ::close(m_wakeupFd);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    assertInLoopThread();
    m_isLooping = true;
    m_isQuited = false;

    LOG_TRACE("EventLoop=0x%x start looping", this);

    while(!m_isQuited) {
        m_activeChannels.clear();
        m_pollReturnTime = m_pollerPtr->poll(&m_activeChannels, kPollTimeMs);
        printActiveChannels();
        m_eventHanding = true;
        for(Channel *channel : m_activeChannels) {
            m_curActiveChannelPtr = channel;
            m_curActiveChannelPtr->handleEvent(m_pollReturnTime);
        }
        m_curActiveChannelPtr = nullptr;
        m_eventHanding = false;
        doPendingFunctors();
    }
    LOG_TRACE("EventLoop=0x%x stop looping", this);
    m_isLooping = false;
}

void EventLoop::quit() {
    m_isQuited = true;
    if(!isInLoopThread) {
        wakeup();
    }
}

void EventLoop::handleRead() {
    std::uint64_t one = 1;
    ssize_t n = sockets::read(m_wakeupFd, &one, sizeof one);
    if (n != sizeof one)    {
        LOG_ERROR("EventLoop::handleRead() reads bytes instead of 8");
    }
}

void EventLoop::printActiveChannels() {
    for(Channel *channel : m_activeChannels) {
        LOG_TRACE("{%s}", channel->events2String().c_str());
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    m_callingPendingFunctors = true;
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        functors.swap(m_pendingFunctors);
    }
    for(std::size_t i = 0; i < functors.size(); ++i) {
        functors[i]();
    }
    m_callingPendingFunctors = false;
}

void EventLoop::wakeup() {
    std::uint64_t one = 1;
    ssize_t n = sockets::write(m_wakeupFd, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::wakeup() writes %d bytes instead of 8", n);
    }
}

void EventLoop::abortNotInLoopThread() {
    std::ostringstream str;
    str << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << m_threadId
            << ", current thread id = " <<  std::this_thread::get_id();
    LOG_FATAL("%s", str.str().c_str());   
}

void EventLoop::runInLoop(const Functor &cb) {
    if(isInLoopThread()) {
        cb();
    }
    else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor &cb) {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_pendingFunctors.push_back(cb);
    }
    if(!isInLoopThread() || m_callingPendingFunctors) {
        wakeup();
    }
}

TimerId EventLoop::runAt(const TimeStamp &when, const TimerCallBack &cb) {
    return m_timerQueuePtr->addTimer(cb, when, 0, 1);
}

TimerId EventLoop::runAfter(std::int64_t delay, const TimerCallBack &cb) {
    TimeStamp time(addTime(TimeStamp::now(), delay));
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(std::int64_t interval, const TimerCallBack &cb) {
    TimeStamp time(addTime(TimeStamp::now(), interval));
    return m_timerQueuePtr->addTimer(std::move(cb), time, interval, -1);
}

void EventLoop::cancle(TimerId id, bool off) {
    m_timerQueuePtr->cancle(id, off);
}

void EventLoop::remove(TimerId id) {
    m_timerQueuePtr->removeTimer(id);
}

void EventLoop::updateChannel(Channel *channel) {
    if(channel->getOwnerLoop() != this) {
        return;
    }
    assertInLoopThread();
    m_pollerPtr->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    if(channel->getOwnerLoop() != this) {
        return;
    }
    assertInLoopThread();
    m_pollerPtr->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
    assertInLoopThread();
    return m_pollerPtr->hasChannel(channel);
}

}   // namespace netlib

}   // namespce YTalk