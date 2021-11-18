/*================================================================================   
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "./AsyncLog.h"

#include <stdio.h>
#include <assert.h>

namespace YTalk
{

AsyncLog::AsyncLog(const int &flush, const off_t &roll, const std::string &path)
    : _flushInterval(flush),
      _rollSize(roll),
      _logFilePath(path),
      _working(false),
      _logThreadID(0),
      _mutex(),
      _cond(),
      _countDown(),
      _curBuffer(new Buffer),
      _nexBuffer(new Buffer),
      _buffers()
{
    _curBuffer->bzero();
    _nexBuffer->bzero();
    _buffers.reserve(16);
}

AsyncLog::~AsyncLog() {
    if(_working) {
        stop();
    }
}

void AsyncLog::start() {
    _working = true;
    if(bthread_start_background(&_logThreadID, nullptr, asyncLogThread, this) != 0) {
        ::fprintf(stderr, "Fail to create bthread  for log!");
        ::exit(-1);
    }
    _countDown.wait();
}

void AsyncLog::stop() {
    _working = false;
    _cond.notify_one();
    bthread_join(_logThreadID, nullptr);
}

void AsyncLog::append(const char *msg, int len) {
    // thread safly
    if(_curBuffer->vaild() > len) {
        _curBuffer->append(msg, len);
    }
    else {
        _buffers.push_back(std::move(_curBuffer));

        if(_nexBuffer) {
            _curBuffer = std::move(_nexBuffer);
        }
        else {
            _curBuffer.reset(new Buffer);
        }

        _curBuffer->append(msg, len);
        _cond.notify_one();
    }
}

void* AsyncLog::asyncLogThread(void *objPtr) {
    // objPtr is AsyncLog pointer
    // because the function is a static member
    // and we need to call nonstatic members
    AsyncLog *self = static_cast<AsyncLog*>(objPtr);
    assert(self->_working);

    
    //TODO
}

}    // namesapce YTalk