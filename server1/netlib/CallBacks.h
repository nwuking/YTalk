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

namespace YTalk
{

namespace netlib
{

typedef std::function<void()>                           TimerCallBack;

} // namespace netlib

} // namesapce YTalk

#endif   // YTALK_CALLBACKS_H
