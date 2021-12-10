/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_REDISCONN_H
#define YTALK_REDISCONN_H

#include <hiredis/hiredis.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace YTalk
{

class RedisPool;

class RedisConn
{
public:
    RedisConn(RedisPool *pool);
    virtual ~RedisConn();

    int init();

    /// string
    std::string get(const char *key);
    std::string set(const char *key, const char *value);
    std::string setex(const char *key, const char *value, int timeout);
    bool mget(const std::vector<std::string> &keys, std::unordered_map<std::string, std::string> &retValue);
    bool isExits(const char *key);

    /// hash type
    std::string hget(const char *key, const char *field);
    //TODO

private:
    RedisPool *_redisPool;
    redisContext *_rContent;

    uint64_t _last_connect_time;
}; // class RedisConn

}    // namespace YTalk


#endif