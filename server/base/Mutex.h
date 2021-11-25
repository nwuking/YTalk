/*================================================================================   
 *    Date: 2021-11-25
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_MUTEX_H
#define YTALK_MUTEX_H

#include <bthread/bthread.h>

namespace YTalk 
{

class Mutex
{
public:
    Mutex() : _ower(0) {
        pthread_mutex_init(&_mutex, NULL);
    }
    ~Mutex() {
        pthread_mutex_destroy(&_mutex);
    }

    void lock() {
        pthread_mutex_lock(&_mutex);
        //_ower = threadId::tid();
    }
    void unlock() {
        _ower = 0;
        pthread_mutex_unlock(&_mutex);
    }

    pthread_mutex_t *mutex() {
        return &_mutex;
    }
private:
    pthread_mutex_t _mutex;
    pid_t _ower;
};     // class Mutex

class MutexLock
{
public:
    MutexLock(Mutex &mutex) : _mutex(mutex) {
        _mutex.lock();
    }
    ~MutexLock() {
        _mutex.unlock();
    }
private:
    Mutex &_mutex;
};    // class MutexLock


}    // namespace YTalk


#endif