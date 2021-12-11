/*================================================================================   
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_REDISSERVICE_H
#define YTALK_REDISSERVICE_H

#include "protobuf/redis.pb.h"

#include <unordered_map>

/// for test
#include <string>
#include "base/Logging.h"
#include "RedisPool.h"
#include "RedisConn.h"
/// for test end

namespace YTalk
{

class RedisPool;
class ConfigParse;

class RedisServiceImpl : public ::DBProxyServer::RedisService
{
public:
    RedisServiceImpl();
    virtual ~RedisServiceImpl();

    virtual void Request(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::RedisRequest* request,
                       ::DBProxyServer::RedisResponse* response,
                       ::google::protobuf::Closure* done);
    
    int init(const std::string &configFile);
    int init(ConfigParse *cParse);
    //TODO

    /// for test
    std::string get(const std::string &redis, const std::string &key) {
        std::unordered_map<std::string, RedisPool*>::iterator it = _redis_pool_map.find(redis);
        if(it == _redis_pool_map.end()) {
            return std::string();
        }

        RedisPool *p = it->second;
        RedisConn *c = p->getRedisConn();
        std::string ret = c->get(key.c_str());
        p->retRedisConn(c);
        return ret;
    }
    std::string set(const std::string &redis, const std::string &key, const std::string &value) {
        std::unordered_map<std::string, RedisPool*>::iterator it = _redis_pool_map.find(redis);
        if(it == _redis_pool_map.end()) {
            return std::string();
        }

        RedisPool *p = it->second;
        RedisConn *c = p->getRedisConn();
        std::string ret = c->set(key.c_str(), value.c_str());
        p->retRedisConn(c);
        return ret;
    }
    /// for test end

private:
    std::unordered_map<std::string, RedisPool*> _redis_pool_map;
};    // class RedisServiceImpl

}    // namespace YTalk


#endif