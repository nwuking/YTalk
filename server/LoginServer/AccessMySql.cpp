/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "AccessMySql.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"
#include "DBProxyServer/protobuf/mysql.pb.h"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#define DB_PROXY_SERVER_IP "db_proxy_server_ip"
#define DB_PROXY_SERVER_PORT "db_proxy_server_port"
#define MAX_RETRY "max_retry"
#define TIMEOUT_MS "timeout_ms"
/// connection_type = [single, pooled, short]
#define CONNECTION_TYPE ""

namespace YTalk
{

AccessMySql::AccessMySql() {
    //TODO
}

AccessMySql::~AccessMySql() {
    //TODO
}

int AccessMySql::init(ConfigParse *cParse) {
    if(!cParse->status()) {
        return 1;
    }

    cParse->getValue(DB_PROXY_SERVER_IP, _dbProxyServer_ip);
    cParse->getValue(DB_PROXY_SERVER_PORT, _dbProxyServer_port_str);
    if(_dbProxyServer_port_str.empty() || _dbProxyServer_ip.empty()) {
        LOG(ERROR) << "Not configure ip or port of DB_Proxy_server";
        return 2;
    }
    std::string server_ip_and_port = _dbProxyServer_ip + ":" + _dbProxyServer_port_str;

    brpc::ChannelOptions options;

    std::string max_retry_str;
    cParse->getValue(MAX_RETRY, max_retry_str);
    if(!max_retry_str.empty()) {
        options.max_retry = std::stoi(max_retry_str);
    } 

    std::string timeout_ms_str;
    cParse->getValue(TIMEOUT_MS, timeout_ms_str);
    if(!timeout_ms_str.empty()) {
        options.timeout_ms = std::stoi(timeout_ms_str);
    }

    std::string connection_type;
    cParse->getValue(CONNECTION_TYPE, connection_type);
    if(!connection_type.empty()) {
        options.connection_type = connection_type;
    }

    if(_channel.Init(server_ip_and_port.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initalize";
        return 3;
    }
    return 0;
}

int AccessMySql::queryForLogin(const std::string &username, const std::string &password) {
    ::DBProxyServer::MySqlRequest request;
    ::DBProxyServer::MySqlResponse response;
    brpc::Controller cntl;

    rapidjson::Document document;
    document.SetObject();

    rapidjson::Value username_v;
    rapidjson::Value password_v;
    username_v.SetString(username.c_str(), username.size(), document.GetAllocator());
    password_v.SetString(password.c_str(), password.size(), document.GetAllocator());
    document.AddMember("username", username_v, document.GetAllocator());
    document.AddMember("password", password_v, document.GetAllocator());

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);

    ::DBProxyServer::MySqlService_Stub stub(&_channel);
    request.set_message(sb.GetString());

    stub.Login(&cntl, &request, &response, nullptr);

    if(!cntl.Failed()) {
        // check successful
        LOG(INFO) << "MySqlService::Login success.";
        return response.status();
    }

    LOG(ERROR) << "MySqlService::Login fail";
    return LOGIN_RPC_FAIL;
}

int AccessMySql::updateForRegister(const UserRegisterInfo &register_info) {
    ::DBProxyServer::MySqlRequest request;
    ::DBProxyServer::MySqlResponse response;
    ::brpc::Controller cntl;

    ::rapidjson::Document document;
    document.SetObject();
    return 0;
    //TODO
}

}    // namesapce YTalk