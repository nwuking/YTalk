/*================================================================================   
 *    Date: 2022-2-3
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "AccessRedis.h"
#include "Channel.h"
#include "base/structs.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "DBProxyServer/protobuf/redis.pb.h"

namespace YTalk
{

AccessRedis::AccessRedis() {
    //TODO
}

AccessRedis::~AccessRedis() {
    //TODO
}

int AccessRedis::init(Channel *channel) {
    if(!channel || !channel->getStatus()) {
        return 1;
    }
    _channel = channel;
    return 0;
}

int AccessRedis::queryForOnline(const std::string &u_name) {
    DBProxyServer::RedisRequest request;
    DBProxyServer::RedisResponse response;
    brpc::Controller cntl;

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Value value;
    value.SetString(u_name.c_str(), u_name.size(), document.GetAllocator());
    document.AddMember(U_NAME, value, document.GetAllocator());

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);

    DBProxyServer::RedisService_Stub stub(_channel->getChannel());
    request.set_message(sb.GetString());

    stub.ReqForOnline(&cntl, &request, &response, nullptr);
    if(!cntl.Failed()) {
        return response.status();
    }

    LOG(ERROR) << "Fail to call RedisService:queryForOnline";
    return -1;
}

int AccessRedis::queryForToken(const std::string &u_name, std::string &token) {
    DBProxyServer::RedisRequest request;
    DBProxyServer::RedisResponse response;
    brpc::Controller cntl;

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Value value;
    value.SetString(u_name.c_str(), u_name.size(), document.GetAllocator());
    document.AddMember(U_NAME, value, document.GetAllocator());

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);

    DBProxyServer::RedisService_Stub stub(_channel->getChannel());
    request.set_message(sb.GetString());

    stub.ReqForToken(&cntl, &request, &response, nullptr);
    if(!cntl.Failed()) {
        if(response.status() == REDIS_SUCCESS) {
            token = response.message();
        }
        return response.status();
    }

    LOG(ERROR) << "Fail to call RedisService:queryForToken";
    return REDIS_SERVER_ERROR;
}

int AccessRedis::setToken(const std::string &u_name, const std::string &token) {
    DBProxyServer::RedisRequest request;
    DBProxyServer::RedisResponse response;
    brpc::Controller cntl;

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Value value;
    value.SetString(u_name.c_str(), u_name.size(), document.GetAllocator());
    document.AddMember(U_NAME, value, document.GetAllocator());

    rapidjson::Value value2;
    value2.SetString(token.c_str(), token.size(), document.GetAllocator());
    document.AddMember(U_TOKEN, value2, document.GetAllocator());

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);

    DBProxyServer::RedisService_Stub stub(_channel->getChannel());
    request.set_message(sb.GetString());

    stub.SetToken(&cntl, &request, &response, nullptr);
    if(!cntl.Failed()) {
        return response.status();
    }

    LOG(ERROR) << "Fail to call RedisService:queryForToken";
    return REDIS_SERVER_ERROR;
}

int AccessRedis::setOnlineStatus(const std::string &u_name, bool flag) {
    DBProxyServer::RedisRequest request;
    DBProxyServer::RedisResponse response;
    brpc::Controller cntl;

    rapidjson::Document document;
    document.SetObject();
    rapidjson::Value value;
    value.SetString(u_name.c_str(), u_name.size(), document.GetAllocator());
    document.AddMember(U_NAME, value, document.GetAllocator());

    rapidjson::Value value2;
    value2.SetBool(flag);
    document.AddMember(U_ONLINE_STATUS, value2, document.GetAllocator());

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);

    DBProxyServer::RedisService_Stub stub(_channel->getChannel());
    request.set_message(sb.GetString());

    stub.SetOnlineStatus(&cntl, &request, &response, nullptr);
    if(!cntl.Failed()) {
        return response.status();
    }

    LOG(ERROR) << "Fail to call RedisService:queryForToken";
    return -1;
}


}   /// namespace YTalk