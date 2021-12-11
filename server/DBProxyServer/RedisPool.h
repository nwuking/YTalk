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
    RedisPool(const std::string &poolName, const std::string &redis_host, uint16_t redis_port, int redis_db, int redis_maxconncnt);
    virtual ~RedisPool();

    int init();

    RedisConn* getRedisConn();
    void retRedisConn(RedisConn *redisConn);

    const char* getRedisHost() {
        return _redis_host.c_str();
    }
    int getRedisPort() {
        return _redis_port;
    }
    int getRedisDbNum() {
        return _redis_db;
    }
    int getRedisMaxConnCnt() {
        return _redis_maxconncnt;
    }
    //TODO

private:
    std::string _pool_name;
    std::string _redis_host;
    int _redis_port;
    int _redis_db;
    int _redis_maxconncnt;
    int _redis_curconncnt;
    std::list<RedisConn*> _free_map;
    Mutex _mutex;
    Cond _cond;

};   // class RedisPool

}    // namespace YTalk

#endif