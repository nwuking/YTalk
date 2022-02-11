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

#include "ThreadUtils.h"

#include <sys/syscall.h>
#include <unistd.h>

namespace YTalk
{

namespace base
{

__thread pid_t t_tid = 0;

static pid_t gettid() {
    return static_cast<pid_t>((syscall(SYS_gettid)));
}

pid_t getCurrentThreadId() {
    if(t_tid == 0) {
        t_tid = gettid();
    }
    return t_tid;
}

}  //namespace base

}  // namesapce Ytalk