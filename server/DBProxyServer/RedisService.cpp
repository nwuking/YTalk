/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "RedisService.h"
#include "RedisPool.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"

#include <vector>

#define CACHEINSTANCES_KEY "RedisInstances"

namespace YTalk
{

RedisServiceImpl::RedisServiceImpl() {
    //TODO
}

RedisServiceImpl::~RedisServiceImpl() {
    std::unordered_map<std::string, RedisPool*>::iterator it = _redis_pool_map.begin();
    for(; it != _redis_pool_map.end(); ++it) {
        RedisPool *p = it->second;
        it->second = nullptr;
        delete p;
    }
}

void RedisServiceImpl::Request(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::RedisRequest* request,
                       ::DBProxyServer::RedisResponse* response,
                       ::google::protobuf::Closure* done) 
{
    //TODO
}

int RedisServiceImpl::init(const std::string &configFile) {
    ConfigParse *cParse = new ConfigParse;
    if(!cParse) {
        LOG(ERROR) << "new ConfigParse failed";
        return 1;
    }

    cParse->parse(configFile);
    if(!cParse->status()) {
        LOG(ERROR) << "ConfigParse pase: (" << configFile << ") failed";
        return 2;
    }

    int s = init(cParse);
    delete cParse;
    return s;
}

int RedisServiceImpl::init(ConfigParse *cParse) {
    std::vector<std::string> redisInstances;
    cParse->getValue(CACHEINSTANCES_KEY, redisInstances);
    if(redisInstances.empty()) {
        LOG(INFO) << "not configure RedisInstances";
        return 3;
    }

    char host[64];
    char port[64];
    char db[64];
    char maxconncnt[64];

    for(auto &poolName : redisInstances) {
        snprintf(host, 64, "%s_host", poolName.c_str());
        snprintf(port, 64, "%s_port", poolName.c_str());
        snprintf(db, 64, "%s_db", poolName.c_str());
        snprintf(maxconncnt, 64, "%s_maxconncnt", poolName.c_str());

        std::string redis_host, redis_port_str, redis_db_str, redis_maxconncnt_str;

        cParse->getValue(host, redis_host);
        cParse->getValue(port, redis_port_str);
        cParse->getValue(db, redis_db_str);
        cParse->getValue(maxconncnt, redis_maxconncnt_str);

        if(redis_host.empty() || redis_port_str.empty() || redis_db_str.empty() || redis_maxconncnt_str.empty()) {
            LOG(ERROR) << "not configure redis instance: " << poolName;
            return 4;
        }

        RedisPool *redisPool = new RedisPool(poolName, redis_host, ::atoi(redis_port_str.c_str()),
                                            ::atoi(redis_db_str.c_str()), ::atoi(redis_maxconncnt_str.c_str()));

        if(!redisPool || redisPool->init()) {
            LOG(ERROR) << "Faild to init RedisPool: " << poolName;
            return 4;
        }

        _redis_pool_map.insert(std::make_pair(poolName, redisPool));
    }
    return 0;
}

}    // namespace YTalk