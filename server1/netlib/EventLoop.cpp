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

namespace YTalk
{

namespace netlib
{

EventLoop::EventLoop() : 
    m_isLooping(false), 
    m_isQuited(false), 
    m_eventHanding(false), 
    m_threadId(std::this_thread::get_id())
{
    //TODO
}

}   // namespace netlib

}   // namespce YTalk