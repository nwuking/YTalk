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
    m_mutex()
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
    //TODO
}

void EventLoop::doPendingFunctors() {
    //TODO
}

void EventLoop::wakeup() {
    //TODO
}

void EventLoop::abortNotInLoopThread() {
    //TODO
}

}   // namespace netlib

}   // namespce YTalk