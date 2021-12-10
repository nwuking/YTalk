/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "RedisPool.h"
#include "RedisConn.h"
#include "base/Logging.h"

#define CACHE_CUR_CONN_CNT 2

namespace YTalk
{

RedisPool::RedisPool(const std::string &poolName, const std::string &cache_host, 
                    uint16_t cache_port, int cache_db, int cache_maxconncnt)
    : _pool_name(poolName), 
      _cache_host(cache_host),
      _cache_port(cache_port),
      _cache_db(cache_db),
      _cache_maxconncnt(cache_maxconncnt),
      _mutex(),
      _cond(_mutex)
{
    _cache_curconncnt = CACHE_CUR_CONN_CNT;
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
    for(int i = 0; i < _cache_curconncnt; ++i) {
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
//TODO

}    // namespace YTalk