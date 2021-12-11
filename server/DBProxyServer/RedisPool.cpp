/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "RedisPool.h"
#include "RedisConn.h"
#include "base/Logging.h"

#define REDIS_CUR_CONN_CNT 2

namespace YTalk
{

RedisPool::RedisPool(const std::string &poolName, const std::string &redis_host, 
                    uint16_t redis_port, int redis_db, int redis_maxconncnt)
    : _pool_name(poolName), 
      _redis_host(redis_host),
      _redis_port(redis_port),
      _redis_db(redis_db),
      _redis_maxconncnt(redis_maxconncnt),
      _mutex(),
      _cond(_mutex)
{
    _redis_curconncnt = REDIS_CUR_CONN_CNT;
}

RedisPool::~RedisPool() {
    if(!_free_map.empty()) {
        std::list<RedisConn*>::iterator it = _free_map.begin();
        for(; it != _free_map.end(); ++it) {
            RedisConn *p = *it;
            *it = nullptr;
            delete p;
        }

        _free_map.clear();
    }
}

int RedisPool::init() {
    for(int i = 0; i < _redis_curconncnt; ++i) {
        RedisConn *p = new RedisConn(this);
        if(p->init()) {
            LOG(ERROR) << "RedisConn init failed in: " << _pool_name;
            delete p;
            return 1;;
        }

        _free_map.push_back(p);
    }

    LOG(INFO) << "cache pool: " << _pool_name << ", size: " << _free_map.size();
    return 0;
}

RedisConn* RedisPool::getRedisConn() {
    MutexLock lock(_mutex);

    while(_free_map.empty()) {
        if(_redis_curconncnt >= _redis_maxconncnt) {
            _cond.wait();
        }
        else {
            RedisConn *newRedisConn = new RedisConn(this);

            if(newRedisConn->init()) {
                LOG(ERROR) << "new RedisConn init failed: " << _pool_name;
                delete newRedisConn;
                return nullptr;
            }

            _free_map.push_back(newRedisConn);
            ++_redis_curconncnt;
            LOG(INFO) << "new RedisConn: " << _pool_name << ", conn_cnt: " << _redis_curconncnt;
        }
    }

    RedisConn *p = _free_map.front();
    _free_map.pop_front();

    return p;
}

void RedisPool::retRedisConn(RedisConn *redisConn) {
    MutexLock lock(_mutex);

    std::list<RedisConn*>::iterator it = _free_map.begin();
    for(; it != _free_map.end(); ++it) {
        if(*it == redisConn) {
            break;
        }
    }

    if(it == _free_map.end()) {
        _free_map.push_back(redisConn);
    }

    _cond.notify();
}
//TODO

}    // namespace YTalk