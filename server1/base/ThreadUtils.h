/**
 * @file ThreadUtils.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_THREAD_UTILS_H
#define YTALK_THREAD_UTILS_H

#include <sys/types.h>

namespace YTalk
{

namespace base
{

pid_t getCurrentThreadId();

}  //namespace base

}   // namesapce Ytalk

#endif