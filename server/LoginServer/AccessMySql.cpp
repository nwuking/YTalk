/*================================================================================   
 *    Date: 2021-12-18
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "AccessMySql.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"
#include "base/structs.h"
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

NewUserInfo* AccessMySql::updateForRegister(const UserRegisterInfo &userRegisterInfo) {
    ::DBProxyServer::MySqlRequest request;
    ::DBProxyServer::MySqlResponse response;
    ::brpc::Controller cntl;

    ::rapidjson::Document document;
    document.SetObject();

/*struct UserRegisterInfo {
    int u_id;
    std::string u_name;
    std::string u_nickname;
    std::string u_password;
    std::string u_gender;
    int u_birthday;
    std::string u_signature;
    //TODO
};*/
    rapidjson::Value u_id;
    u_id.SetInt(userRegisterInfo.u_id);
    document.AddMember(U_ID, u_id, document.GetAllocator());

    rapidjson::Value u_name;
    u_name.SetString(userRegisterInfo.u_name.c_str(), userRegisterInfo.u_name.size(), document.GetAllocator());
    document.AddMember(U_NAME, u_name, document.GetAllocator());

    rapidjson::Value u_nickname;
    u_nickname.SetString(userRegisterInfo.u_nickname.c_str(), userRegisterInfo.u_nickname.size(), document.GetAllocator());
    document.AddMember(U_NICKNAME, u_nickname, document.GetAllocator());

    rapidjson::Value u_password;
    u_password.SetString(userRegisterInfo.u_password.c_str(), userRegisterInfo.u_password.size(), document.GetAllocator());
    document.AddMember(U_PASSWORD, u_password, document.GetAllocator());

    rapidjson::Value u_gender;
    u_gender.SetString(userRegisterInfo.u_gender.c_str(), userRegisterInfo.u_gender.size(), document.GetAllocator());
    document.AddMember(U_GENDER, u_gender, document.GetAllocator());

    rapidjson::Value u_birthday;
    u_birthday.SetInt(userRegisterInfo.u_birthday);
    document.AddMember(U_BIRTHDAY, u_birthday, document.GetAllocator());

    /*if(!userRegisterInfo.u_signature.empty()) {
        rapidjson::Value u_signature;
        u_signature.SetString(userRegisterInfo.u_signature.c_str(), userRegisterInfo.u_signature.size(), document.GetAllocator());
        document.AddMember(U_SIGNATURE, u_signature, document.GetAllocator());
    }*/

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);

    DBProxyServer::MySqlService_Stub stub(&_channel);
    request.set_message(sb.GetString());

    stub.Register(&cntl, &request, &response, nullptr);
    if(!cntl.Failed()) {
        NewUserInfo *result = new NewUserInfo;
        if(response.status() == DBPROXY_SUCCESS) {
            result->status = 0;
            result->u_id = userRegisterInfo.u_id;
        }
        else {
            result->status = 1;
            result->u_id = -1;
        }
        return result;
    }

    LOG(ERROR) << "Call MySqlService::Register fail";
    return nullptr;
}

int AccessMySql::getMaxUserId() {
    DBProxyServer::MySqlRequest request;
    DBProxyServer::MySqlResponse response;
    brpc::Controller cntl;

    DBProxyServer::MySqlService_Stub stub(&_channel);

    request.set_message("1");
    stub.GetMaxUserId(&cntl, &request, &response, nullptr);

    if(!cntl.Failed()) {
        if(response.status() == DBPROXY_SUCCESS) {
            return std::stoi(response.message());
        }
        else {
            LOG(ERROR) << "AccessMySql::getMaxUserId: DBProxy fail";
        }
    }
    else {
        LOG(ERROR) << "AccessMySql::getMaxUserId: call rpc fail";
    }
    return -1;
}

}    // namesapce YTalk