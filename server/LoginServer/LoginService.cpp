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

namespace YTalk
{

LoginServiceImpl::LoginServiceImpl() {
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
    //TODO
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

    //TODO: 从user中获取u_id最大值
    return 0;
}

}    //   namespace YTalk