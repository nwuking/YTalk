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

    virtual void ReqForOnline(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::RedisRequest* request,
                       ::DBProxyServer::RedisResponse* response,
                       ::google::protobuf::Closure* done);

    virtual void ReqForToken(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::RedisRequest* request,
                       ::DBProxyServer::RedisResponse* response,
                       ::google::protobuf::Closure* done);

    virtual void SetToken(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::RedisRequest* request,
                       ::DBProxyServer::RedisResponse* response,
                       ::google::protobuf::Closure* done);
    
    int init(const std::string &configFile);
    int init(ConfigParse *cParse);
    //TODO

private:
    std::unordered_map<std::string, RedisPool*> _redis_pool_map;
};    // class RedisServiceImpl

}    // namespace YTalk


#endif