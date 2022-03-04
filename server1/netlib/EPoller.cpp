/**
 * @file Poller.cpp
 * @author nuwking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "EPoller.h"
#include "EventLoop.h"
#include "Channel.h"
#include "../base/Logger.h"

#include <cstring>
#include <unistd.h>

namespace YTalk
{

using namespace base;

namespace netlib
{

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPoller::EPoller(EventLoop *loop) : 
    m_loopPtr(loop), 
    m_epollFd(::epoll_create1(EPOLL_CLOEXEC)), 
    m_events(s_kInitEventArrSize)
{
    if(m_epollFd < 0) {
        LOG_FATAL("EPoller::EPoller()");
    }
}

EPoller::~EPoller() {
    ::close(m_epollFd);
}

TimeStamp EPoller::poll(CHANNEL_ARR *activeChannels, int timeout) {
    //LOG_TRACE("")
    int numEvents = ::epoll_wait(m_epollFd, &*m_events.begin(), static_cast<int>(m_events.size()), timeout);
    int savedErrno = errno;

    TimeStamp now(TimeStamp::now());
    if(numEvents > 0) {
        // 有事件响应
        LOG_TRACE("%d events happend");
        fillActiveChannels(activeChannels, numEvents);
        if(static_cast<std::size_t>(numEvents) == m_events.size()) {
            m_events.resize(m_events.size()*2);
        }
       
    }
    else if(numEvents == 0) {
        LOG_TRACE("nothing happend");        
    }
    else {
        if(errno != EINTR) {
            // 不是发生终端
            errno = savedErrno;
            LOG_SYSERROR("EPoller::poll() error");
        }
    }
    return now;
}

void EPoller::updateChannel(Channel *channel) {
    assertInLoopThread();

    LOG_TRACE("fd=%d, events=%d ", channel->fd(), channel->events());

    const int index = channel->index();
    int fd = channel->fd();
    if(index == kNew || index == kDeleted) {
        if(index == kNew) {
            if(m_channels.find(fd) != m_channels.end()) {
                LOG_ERROR("fd=%d must not exist in m_channels", fd);
                return;
            }
            m_channels.insert(std::make_pair(fd, channel));
        }
        else {
            if(m_channels.find(fd) == m_channels.end()) {
                LOG_ERROR("fd=%d must exist in m_channels", fd);
                return;
            }
            if(m_channels[fd] != channel) {
                LOG_ERROR("current channel is not matched current fd, fd=%d", fd);
                return;
            }
        }

        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else {
        if(m_channels.find(fd) == m_channels.end() || m_channels[fd] != channel || index != kAdded) {
            LOG_ERROR("current channel is not matched current fd, fd = %d, channel = 0x%x", fd, channel);
            return;
        }

        if(channel->isNoneEvent()) {
            //该Channel没有要监听的事件，remove
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
        else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoller::removeChannel(Channel *channel) {
    assertInLoopThread();

    int fd = channel->fd();
    if(m_channels.find(fd) == m_channels.end() || m_channels[fd] != channel || !channel->isNoneEvent()) {
        return;
    }

    int index = channel->index();
    if(index != kAdded || index != kDeleted) {
        return;
    }

    std::size_t p = m_channels.erase(fd);
    if(p != 1) {
        return;
    }

    if(index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }

    channel->setIndex(kNew);
}

bool EPoller::hasChannel(Channel *channel) const {
    assertInLoopThread();
    CHANNEL_MAP::const_iterator it = m_channels.find(channel->fd());
    return (it != m_channels.end() && it->second == channel);
}

void EPoller::assertInLoopThread() const {
    m_loopPtr->assertInLoopThread();
}

void EPoller::fillActiveChannels(CHANNEL_ARR *activeChannels, int numChannels) {
    for(int i = 0; i < numChannels; ++i) {
        Channel *channel = static_cast<Channel*>(m_events[i].data.ptr);
        CHANNEL_MAP::const_iterator it = m_channels.find(channel->fd());
        if(it == m_channels.end() || it->second != channel) {
            continue;
        }
        channel->setRevents(m_events[i].events);
        activeChannels->push_back(channel);
    }
}

void EPoller::update(int op, Channel *channel) {
    epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    
    LOG_TRACE("epoll_ctl op=%s, fd=%d, events={%s}", op2String(op).c_str(), fd, channel->events2String().c_str());

    if(::epoll_ctl(m_epollFd, op, fd, &event) < 0) {
        if(op == EPOLL_CTL_DEL) {
            LOG_SYSERROR("epoll_ctl op=%s, fd=%d", op2String(op).c_str(), fd);
        }
        else {
            LOG_FATAL("epoll_ctl op=%s, fd=%d", op2String(op).c_str(), fd);
        }
    }
}

std::string EPoller::op2String(int op) {
    if(op == EPOLL_CTL_ADD) return "EPOLL_CTL_ADD";
    if(op == EPOLL_CTL_DEL) return "EPOLL_CTL_DEL";
    if(op == EPOLL_CTL_MOD) return "EPOLL_CTL_MOD";
    return "EPOLL_CTL_NONE";
}

}    //// namespace netlib

}   // namespace YTalk