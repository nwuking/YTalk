/*================================================================================   
 *    Date: 2021-11-19
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "./AsyncLog.h"
#include "./LogFile.h"

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
      _lock(_mutex, std::defer_lock),
      _cond(),
      _countDown(),
      _curBuffer(new Buffer),
      _nexBuffer(new Buffer),
      _buffers()
{
    _curBuffer->bzero();
    _nexBuffer->bzero();
    _buffers.reserve(2);
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
   _lock.lock();
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
    _lock.unlock();
}

void* AsyncLog::asyncLogThread(void *objPtr) {
    // objPtr is AsyncLog pointer
    // because the function is a static member
    // and we need to call nonstatic members
    AsyncLog *self = static_cast<AsyncLog*>(objPtr);
    assert(self->_working);

    LogFile output(self->_flushInterval, self->_rollSize, self->_logFilePath);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVec buffersToWrite;
    buffersToWrite.reserve(2);

    self->_countDown.signal();

    while(self->_working) {
        assert(newBuffer1 && newBuffer1->size() == 0);
        assert(newBuffer2 && newBuffer2->size() == 0);
        assert(buffersToWrite.empty());

        self->_lock.lock();
        if(self->_buffers.empty()) {
            self->_cond.wait_for(self->_lock, self->_flushInterval);
        }
        self->_buffers.push_back(std::move(self->_curBuffer));
        self->_curBuffer = std::move(newBuffer1);
        buffersToWrite.swap(self->_buffers);
        if(!self->_nexBuffer) {
            self->_nexBuffer = std::move(newBuffer2);
        }
        self->_lock.unlock();

        assert(!buffersToWrite.empty());

        for(const auto &buffer : buffersToWrite) {
            output.append(buffer->data(), buffer->size());
        }

        if(buffersToWrite.size() > 2) {
            buffersToWrite.resize(2);
        }

        if(!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();            
        }
        if(!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();            
        }

        buffersToWrite.clear();
        output.flush();
    }

    output.flush();
}

}    // namesapce YTalk