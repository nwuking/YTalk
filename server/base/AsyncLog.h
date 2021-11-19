/*================================================================================   
 *    Date: 2021-11-18
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_ASYNCLOG_H
#define YTALK_ASYNCLOG_H

//#include <butil/iobuf.h>    // brpc
#include <bthread/bthread.h>    // brpc
#include <bthread/condition_variable.h>    // brpc
#include <bthread/countdown_event.h>    // brpc

#include <string>
#include <vector>
#include <memory>
#include <atomic>

#include "./LogBuffer.h"

namespace YTalk
{

//some value by default
static const int FLUSHINTERVAL = 3;    // s
static const int ROLLSIZE = 8000;      // bytes
static const char* LOGFILEPATH = "../data/logs/";    //

class AsyncLog
{
public:
    AsyncLog(const int &flush, const off_t &roll, const std::string &path);
    ~AsyncLog();

    void start();

    void stop();

    void append(const char *msg, int size);

private:
    typedef LogBuffer<klargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVec;
    typedef BufferVec::value_type BufferPtr;

    static void* asyncLogThread(void *);

    int _flushInterval;
    off_t _rollSize;
    std::string _logFilePath;

    std::atomic<bool> _working;

    //
    bthread_t _logThreadID;
    bthread_mutex_t _mutex;
    std::unique_lock<bthread_mutex_t> _lock;
    bthread::ConditionVariable _cond;
    bthread::CountdownEvent _countDown;

    // buffer
    BufferPtr _curBuffer;
    BufferPtr _nexBuffer;
    BufferVec _buffers;

};    // class AsyncLog

}   // namesapce YTalk

#endif