/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_REDISPOOL_H
#define YTALK_REDISPOOL_H

#include "base/Condition.h"
#include "base/Mutex.h"

#include <string>
#include <list>

namespace YTalk
{

class RedisConn;

class RedisPool
{
public:
    RedisPool(const std::string &poolName, const std::string &cache_host, uint16_t cache_port, int cache_db, int cache_maxconncnt);
    virtual ~RedisPool();

    int init();

    const char* getCacheHost() {
        return _cache_host.c_str();
    }
    int getCachePort() {
        return _cache_port;
    }
    int getCacheDbNum() {
        return _cache_db;
    }
    int getCacheMaxConnCnt() {
        return _cache_maxconncnt;
    }
    //TODO

private:
    std::string _pool_name;
    std::string _cache_host;
    int _cache_port;
    int _cache_db;
    int _cache_maxconncnt;
    int _cache_curconncnt;
    std::list<RedisConn*> _free_map;
    Mutex _mutex;
    Cond _cond;

};   // class RedisPool

}    // namespace YTalk

#endif