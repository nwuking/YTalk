/*================================================================================   
 *    Date: 2021-12-21
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "LoginService.h"
#include "base/ConfigParse.h"
#include "AccessMySql.h"
#include "Session.h"
#include "base/Logging.h"
#include "base/structs.h"

//#include <rapidjson/document.h>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#define USERNAME "username"
#define PASSWORD "password"

#define U_NAME "u_name"
#define U_NICKNAME "u_nickname"
#define U_PASSWORD "u_password"
#define U_GENDER "u_gender"
#define U_BIRTHDAY "u_birthday"
#define U_SIGNATURE "u_signature"

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
    ::brpc::ClosureGuard done_guard(done);
    ::brpc::Controller *cntl = static_cast<::brpc::Controller*>(controller);

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

    std::string username, password;
    rapidjson::Value::MemberIterator u = document.FindMember(USERNAME);
    rapidjson::Value::MemberIterator p = document.FindMember(PASSWORD);
    if(u == document.MemberEnd() || p == document.MemberEnd()) {
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_BAD_REQUEST);
        return;
    } 
    username = u->value.GetString();
    password = p->value.GetString();

    int status = _accessMySql->queryForLogin(username, password);

    if(status == LOGIN_SUCCESS) {
        struct GateServerMsg *gate = _session->getGateServerMsg();
        if(!gate) {
            LOG(ERROR) << "There is not GateServer Msg";
            cntl->http_response().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
            return;
        }

        std::string rspMsg = "{\"server_ip\" : \"" + gate->_ip + "\", \"server_port\" : " + std::to_string(gate->_port) + "}";

        //rapidjson::Document document;
        document.Parse(rspMsg.c_str());
        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
        document.Accept(writer);

        rspMsg = sb.GetString();
        cntl->response_attachment().append(rspMsg.c_str());
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_OK);
    }
    else {
        if(status == LOGIN_FAIL) {
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

int LoginServiceImpl::init(ConfigParse *cParse, Session *session) {
    _session = session;
    _accessMySql = new AccessMySql();
    if(!_accessMySql) {
        LOG(ERROR) << "Fail to new AccessMySql";
        return 1;
    }
    if(_accessMySql->init(cParse)) {
        LOG(ERROR) << "Fail to initializate AccessMySql";
        return 2;
    }

    //从user中获取u_id最大值
    _baseUserId = _accessMySql->getMaxUserId();
    if(_baseUserId == -1) {
        LOG(ERROR) << "Fail to init, _baseUserId = -1";
        return 3;
    }
    return 0;
}

}    //   namespace YTalk