/**
 * @file CallBacks.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_CALLBACKS_H
#define YTALK_CALLBACKS_H

#include <functional>
#include <memory>
#include <cstdint>

#include "../base/TimeStamp.h"

namespace YTalk
{

namespace netlib
{

class TcpConnection;
class Buffer;

typedef std::function<void()>                           TimerCallBack;

typedef std::function<void()>                           EventCallBack;
typedef std::function<void(base::TimeStamp)>            ReadEventCallBack;

typedef std::shared_ptr<TcpConnection>                      TcpConnectionPtr;

typedef std::function<void(const TcpConnectionPtr&)>        ConnectionCallBack;
typedef std::function<void(const TcpConnectionPtr&)>        CloseCallBack;
typedef std::function<void(const TcpConnectionPtr&)>        WriteCompleteCallback;
typedef std::function<void(const TcpConnectionPtr&, std::size_t)> HighWaterMarkCallback;

typedef std::function<void(const TcpConnectionPtr&, Buffer*, base::TimeStamp)> MessageCallback;

// 在TcpConnection.cpp 里定义
void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, base::TimeStamp receiveTime);

} // namespace netlib

} // namesapce YTalk

#endif   // YTALK_CALLBACKS_H
