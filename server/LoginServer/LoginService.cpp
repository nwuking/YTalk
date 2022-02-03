/*================================================================================   
 *    Date: 2021-12-21
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "LoginService.h"
#include "base/ConfigParse.h"
#include "AccessMySql.h"
#include "AccessRedis.h"
#include "Session.h"
#include "Channel.h"
#include "base/Logging.h"
#include "base/structs.h"
#include "base/Token.h"

//#include <rapidjson/document.h>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

//#define USERNAME "username"
//#define PASSWORD "password"

namespace YTalk
{

LoginServiceImpl::LoginServiceImpl() :_mutex() {
    _accessMySql = nullptr;
    _session = nullptr;
}

LoginServiceImpl::~LoginServiceImpl() {
    if(_accessMySql) {
        delete _accessMySql;
    }
}

void LoginServiceImpl::Login(::google::protobuf::RpcController* controller,
                       const ::LoginServer::HttpRequest* request,
                       ::LoginServer::HttpResponse* response,
                       ::google::protobuf::Closure* done)
{
    // called by client
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<::brpc::Controller*>(controller);

    std::string reqMsg = cntl->request_attachment().to_string();
    if(reqMsg.empty()) {
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_BAD_REQUEST);
        return;
    }
    rapidjson::Document document;
    if(document.Parse(reqMsg.c_str()).HasParseError()) {
        LOG(ERROR) << "Fail to parse json";
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }

    std::string u_name, u_password;
    rapidjson::Value::MemberIterator u = document.FindMember(U_NAME);
    rapidjson::Value::MemberIterator p = document.FindMember(U_PASSWORD);
    if(u == document.MemberEnd() || p == document.MemberEnd()) {
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_BAD_REQUEST);
        return;
    } 
    u_name = u->value.GetString();
    u_password = p->value.GetString();

    int status = _accessMySql->queryForLogin(u_name, u_password);

    if(status == DBPROXY_SUCCESS) {
        struct GateServerMsg *gate = _session->getGateServerMsg();
        if(!gate) {
            LOG(ERROR) << "There is not GateServer Msg";
            cntl->http_response().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
            return;
        }

        //访问redis，查询该账号是否在线
        int redis_status = _accessRedis->queryForOnline(u_name);
        if(redis_status == REDIS_IS_ONLINE) {
            //TODO:踢人
        }
        else if(redis_status != REDIS_SUCCESS) {
            cntl->http_response().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
            return;
        }

        //查询redis是否有token
        std::string token;
        redis_status = _accessRedis->queryForToken(u_name, token);
        if(redis_status == REDIS_NO_TOKEN) {
            token = TokenGenerator::generateToken(u_name);
        }
        else if(redis_status != REDIS_SUCCESS) {
            cntl->http_response().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
            return;
        }
        
        std::string rspMsg = "{"
                                    "\"server_ip\": \"" + gate->_ip + "\", "
                                    "\"server_port\": \"" + std::to_string(gate->_port) + "\", "
                                    "\"token\": \"" + token + "\""
                             "}"; 

        cntl->response_attachment().append(rspMsg.c_str());
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_OK);
    }
    else {
        if(status == DBPROXY_FAIL) {
            cntl->http_response().set_status_code(brpc::HTTP_STATUS_NOT_FOUND);
        }
        else {
            cntl->http_response().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
        }
    }
}

void LoginServiceImpl::Register(::google::protobuf::RpcController* controller,
                       const ::LoginServer::HttpRequest* request,
                       ::LoginServer::HttpResponse* response,
                       ::google::protobuf::Closure* done)
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);

    std::string reqMsg = cntl->request_attachment().to_string();
    if(reqMsg.empty()) {
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_BAD_REQUEST);
        return;
    }

    rapidjson::Document document;
    if(document.Parse(reqMsg.c_str()).HasParseError()) {
        LOG(ERROR) << "Fail to parse json";
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }

    UserRegisterInfo userRegisterInfo;
    //std::string u_name, u_nickname, u_password;
    rapidjson::Value::MemberIterator it1 = document.FindMember(U_NAME);
    rapidjson::Value::MemberIterator it2 = document.FindMember(U_NICKNAME);
    rapidjson::Value::MemberIterator it3 = document.FindMember(U_PASSWORD);
    if(it1 == document.MemberEnd() || it2 == document.MemberEnd() || it3 == document.MemberEnd()) {
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_BAD_REQUEST);
        return;
    }

    userRegisterInfo.u_name = it1->value.GetString();
    userRegisterInfo.u_nickname = it2->value.GetString();
    userRegisterInfo.u_password = it3->value.GetString();
LOG(INFO) << "KAKAK1";
    //int u_id;
    {
        MutexLock lock(_mutex);
        ++_baseUserId;
        userRegisterInfo.u_id = _baseUserId;
    }

    //std::string u_gender;
    it1 = document.FindMember(U_GENDER);
    if(it1 == document.MemberEnd()) {
        userRegisterInfo.u_gender = "1";
    }
    else {
        userRegisterInfo.u_gender = it1->value.GetString();
    }

    //int u_birthday;
    it1 = document.FindMember(U_BIRTHDAY);
    if(it1 == document.MemberEnd()) {
        userRegisterInfo.u_birthday = 20220101;
    }
    else {
        userRegisterInfo.u_birthday = it1->value.GetInt();
    }

    /*it1 = document.FindMember(U_SIGNATURE);
    if(it1 != document.MemberEnd()) {
        userRegisterInfo.u_signature = it1->value.GetString();
    }*/
LOG(INFO) << "KAKAKA2";
    NewUserInfo *newUser = _accessMySql->updateForRegister(userRegisterInfo);
    if(!newUser) {
        LOG(ERROR) << "NewUserInfo is nullptr";
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return;
    }
    if(newUser->status) {
        //fail
        LOG(ERROR) << "Fail to register";
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
        //TODO
    }
    else {
        //success
        LOG(INFO) << "Success to register: " << newUser->u_id;
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_OK);
        //TODO: 返回一些信息给客户端？
    }
    delete newUser;
}

int LoginServiceImpl::init(ConfigParse *cParse, Session *session, Channel *channel) {
    _session = session;
    _accessMySql = new AccessMySql();
    _accessRedis = new AccessRedis();
    if(!_accessMySql || !_accessRedis) {
        LOG(ERROR) << "Fail to new AccessMySql or AccessRedis";
        return 1;
    }
    if(_accessMySql->init(channel)) {
        LOG(ERROR) << "Fail to initializate AccessMySql";
        return 2;
    }
    if(_accessRedis->init(channel)) {
        LOG(ERROR) << "Fail to init AccessRedis";
        return 3;
    }

    //从user中获取u_id最大值
    _baseUserId = _accessMySql->getMaxUserId();
    if(_baseUserId == -1) {
        LOG(ERROR) << "Fail to init, _baseUserId = -1";
        return 4;
    }
    return 0;
}

}    //   namespace YTalk