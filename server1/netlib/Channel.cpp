/**
 * @file Channel.cpp
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Channel.h"
#include "EventLoop.h"
#include "../base/Logger.h"

#include <poll.h>

#include <sstream>

namespace YTalk
{

namespace netlib
{

using namespace base;

const int Channel::s_kNoneEvent = 0;
const int Channel::s_kReadEvent = POLLIN | POLLPRI;
const int Channel::s_kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd) : 
    m_loop(loop), 
    m_fd(fd),
    m_events(0), 
    m_revents(0),
    m_handingEvent(false),
    m_added2Loop(false),
    m_index(-1)
{
    //TODO
}

Channel::~Channel() {
    //TODO
}

void Channel::handleEvent(TimeStamp receiveTime) {
    m_handingEvent = true;

    LOG_TRACE(revents2String().c_str());
    if((m_revents & POLLHUP) && ! (m_revents & POLLIN)) {
        if(m_closeCallBack) {
            m_closeCallBack();
        }
    }

    if(m_revents & POLLNVAL) {
        LOG_WRANING("fd=%d, Channel::handleEvent() POLLNVAL", m_fd);
    }

    if(m_revents & (POLLERR | POLLNVAL)) {
        if(m_errorCallBack) {
            m_errorCallBack();
        }
    }

    if(m_revents & (POLLIN | POLLPRI | POLLRDHUP)) {
        if(m_readCallBack) {
            m_readCallBack(receiveTime);
        }
    }

    if(m_revents & POLLOUT) {
        if(m_writeCallBack) {
            m_writeCallBack();
        }
    }

    m_handingEvent = false;
}

void Channel::remove() {
    if(!isNoneEvent()) {
        return;
    }

    m_added2Loop = false;
    m_loop->removeChannel(this);
}

void Channel::update() {
    m_added2Loop = true;
    m_loop->updateChannel(this);
}

std::string Channel::revents2String() {
    return events2String(m_fd, m_revents);
}

std::string Channel::events2String(int fd, int ev) {
    std::ostringstream str;
    str << "fd= " << fd << ": ";

    if (ev & POLLIN)
        str << "IN ";
    if (ev & POLLPRI)
        str << "PRI ";
    if (ev & POLLOUT)
        str << "OUT ";
    if (ev & POLLHUP)
        str << "HUP ";
    if (ev & POLLRDHUP)
        str << "RDHUP ";
    if (ev & POLLERR)
        str << "ERR ";
    if (ev & POLLNVAL)
        str << "NVAL ";

  return str.str();
}

}    //// namespace netlib

}   // namespace YTalk