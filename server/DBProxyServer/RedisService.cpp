/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "RedisService.h"
#include "RedisPool.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"
#include "base/structs.h"
#include "rapidjson/document.h"

#include <vector>
#include <brpc/channel.h>

#define CACHEINSTANCES_KEY "RedisInstances"
#define ONLINE "online"
#define TOKEN "token"

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

void RedisServiceImpl::ReqForOnline(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::RedisRequest* request,
                       ::DBProxyServer::RedisResponse* response,
                       ::google::protobuf::Closure* done) 
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);

    rapidjson::Document document;
    document.Parse(request->message().c_str());
    if(!document.HasMember(U_NAME)) {
        response->set_status(REDIS_CLIENT_ERROR);
        return;
    }

    std::string u_name = document[U_NAME].GetString();

    std::unordered_map<std::string, RedisPool*>::iterator it = _redis_pool_map.find(ONLINE);
    if(it == _redis_pool_map.end()) {
        LOG(ERROR) << "Defect db: " << ONLINE;
        response->set_status(REDIS_SERVER_ERROR);
        return;
    }

    RedisPool *pool = it->second;
    RedisConn *conn = pool->getRedisConn();
    if(!conn) {
        LOG(ERROR) << "RedisConn unuseful in:" << pool->getRedisDbNum();
        response->set_status(REDIS_SERVER_ERROR);
        return;
    }

    if(conn->isExits(u_name.c_str())) {
        std::string res = conn->get(u_name.c_str());
        if(res == "online") {
            response->set_status(REDIS_IS_ONLINE);
        }
        else {
            response->set_status(REDIS_SUCCESS);
        }
    }
    else {
        std::string res = conn->set(u_name.c_str(), "offline");
        response->set_status(REDIS_SUCCESS);
    }
    
    pool->retRedisConn(conn);
}

void RedisServiceImpl::ReqForToken(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::RedisRequest* request,
                       ::DBProxyServer::RedisResponse* response,
                       ::google::protobuf::Closure* done) 
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);

    rapidjson::Document document;
    document.Parse(request->message().c_str());
    if(!document.HasMember(U_NAME)) {
        response->set_status(REDIS_CLIENT_ERROR);
        return;
    }

    std::string u_name = document[U_NAME].GetString();

    std::unordered_map<std::string, RedisPool*>::iterator it = _redis_pool_map.find(TOKEN);
    if(it == _redis_pool_map.end()) {
        LOG(ERROR) << "Defect db: " << TOKEN;
        response->set_status(REDIS_SERVER_ERROR);
        return;
    }

    RedisPool *pool = it->second;
    RedisConn *conn = pool->getRedisConn();
    if(!conn) {
        LOG(ERROR) << "RedisConn unuseful in:" << pool->getRedisDbNum();
        response->set_status(REDIS_SERVER_ERROR);
        return;
    }

    if(conn->isExits(u_name.c_str())) {
        std::string token = conn->get(u_name.c_str());
        response->set_message(token);
        response->set_status(REDIS_SUCCESS);
    }
    else {
        response->set_status(REDIS_NO_TOKEN);
    }

    pool->retRedisConn(conn);
}

void RedisServiceImpl::SetToken(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::RedisRequest* request,
                       ::DBProxyServer::RedisResponse* response,
                       ::google::protobuf::Closure* done) 
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);
/*{
    "u_name": ****,
    "token": *****
}*/
    rapidjson::Document document;
    document.Parse(request->message().c_str());
    if(!document.HasMember(U_NAME) || !document.HasMember(U_TOKEN)) {
        response->set_status(REDIS_CLIENT_ERROR);
        return;
    }

    std::string u_name = document[U_NAME].GetString();
    std::string token = document[U_TOKEN].GetString();

    std::unordered_map<std::string, RedisPool*>::iterator it = _redis_pool_map.find(TOKEN);
    if(it == _redis_pool_map.end()) {
        LOG(ERROR) << "Defect db: " << TOKEN;
        response->set_status(REDIS_SERVER_ERROR);
        return;
    }

    RedisPool *pool = it->second;
    RedisConn *conn = pool->getRedisConn();
    if(!conn) {
        LOG(ERROR) << "RedisConn unuseful in:" << pool->getRedisDbNum();
        response->set_status(REDIS_SERVER_ERROR);
        return;
    }

    //7天免登录
    int timeout = 7 * 24 * 60 * 60;
    std::string retValue = conn->setex(u_name.c_str(), token.c_str(), timeout);
    response->set_status(REDIS_SUCCESS);

    pool->retRedisConn(conn);
}

void RedisServiceImpl::SetOnlineStatus(::google::protobuf::RpcController* controller,
                       const ::DBProxyServer::RedisRequest* request,
                       ::DBProxyServer::RedisResponse* response,
                       ::google::protobuf::Closure* done) 
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);
/*{
    "u_name": ****,
    "online_status": *****
}*/
    rapidjson::Document document;
    document.Parse(request->message().c_str());
    if(!document.HasMember(U_NAME) || !document.HasMember(U_ONLINE_STATUS)) {
        response->set_status(REDIS_CLIENT_ERROR);
        return;
    }

    std::string u_name = document[U_NAME].GetString();
    bool u_online_status = document[U_ONLINE_STATUS].GetBool();

    std::unordered_map<std::string, RedisPool*>::iterator it = _redis_pool_map.find(ONLINE);
    if(it == _redis_pool_map.end()) {
        LOG(ERROR) << "Defect db: " << ONLINE;
        response->set_status(REDIS_SERVER_ERROR);
        return;
    }

    RedisPool *pool = it->second;
    RedisConn *conn = pool->getRedisConn();
    if(!conn) {
        LOG(ERROR) << "RedisConn unuseful in:" << pool->getRedisDbNum();
        response->set_status(REDIS_SERVER_ERROR);
        return;
    }

    std::string redis_status = conn->set(u_name.c_str(), (u_online_status ? "online" : "offline"));
    response->set_status(REDIS_SUCCESS);

    pool->retRedisConn(conn);
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