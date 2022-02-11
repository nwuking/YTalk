/*================================================================================   
 *    Date: 2021-12-20
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "AuthService.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"
#include "Session.h"
#include "base/structs.h"
#include "AccessRedis.h"
#include "Channel.h"

#include "brpc/server.h"
#include "rapidjson/document.h"

#define GATE_SERVER_NAME "name"
#define GATE_SERVER_PORT "port"
#define GATE_SERVER_CLIENTS "counts"

namespace YTalk
{

AuthServiceImpl::AuthServiceImpl() {
    _session = nullptr;
    _accessRedis = nullptr;
}

AuthServiceImpl::~AuthServiceImpl() {
    delete _accessRedis;
}

void AuthServiceImpl::Auth(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done)
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);

    std::string reqMsg = request->message();
    rapidjson::Document document;
    if(document.Parse(reqMsg.c_str()).HasParseError()) {
        LOG(ERROR) << "Fail to parse json: Auth";
        return;
    }
/*
{
    "u_name": ****
    "token": *****
}
*/
    std::string u_name, token;
    rapidjson::Value::MemberIterator u = document.FindMember(U_NAME);
    rapidjson::Value::MemberIterator t = document.FindMember(U_TOKEN);
    if(u == document.MemberEnd() || t == document.MemberEnd()) {
        response->set_status(LOGIN_FAIL);
        return;
    }

    u_name = u->value.GetString();
    token = t->value.GetString();

    std::string theToken;
    if(_accessRedis->queryForToken(u_name, theToken) == REDIS_SUCCESS && token == theToken) {
        response->set_status(LOGIN_SUCCESS);
        return;
    }
    response->set_status(REDIS_SERVER_ERROR);
}

void AuthServiceImpl::Notify(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done)
{
    ::brpc::ClosureGuard done_guard(done);
    ::brpc::Controller *cntl = static_cast<::brpc::Controller*>(controller);

    std::string reqMsg = request->message();
    rapidjson::Document document;
    if(document.Parse(reqMsg.c_str()).HasParseError()) {
        LOG(ERROR) << "Fail to parse json: Notify";
        return;
    }

    std::string gate_server_name;
    int gate_srever_clients;
    rapidjson::Value::MemberIterator name = document.FindMember(GATE_SERVER_NAME);
    rapidjson::Value::MemberIterator counts = document.FindMember(GATE_SERVER_CLIENTS);
    if(name == document.MemberEnd() || counts == document.MemberEnd()) {
        return;
    }

    gate_server_name = name->value.GetString();
    gate_srever_clients = counts->value.GetInt();

    _session->record(gate_server_name, gate_srever_clients);
}

void AuthServiceImpl::FirstSend(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done)
{
    ::brpc::ClosureGuard done_guard(done);
    ::brpc::Controller *cntl = static_cast<::brpc::Controller*>(controller);

    std::string reqMsg = request->message();
    rapidjson::Document document;
    if(document.Parse(reqMsg.c_str()).HasParseError()) {
        LOG(ERROR) << "Fail to parse json: FirstSend";
        return;
    }
/*
{
    "name" : server_name
    "port" : server_port
}
*/
    std::string gate_server_name;
    int gate_server_port;
    rapidjson::Value::MemberIterator name = document.FindMember(GATE_SERVER_NAME);
    rapidjson::Value::MemberIterator port = document.FindMember(GATE_SERVER_PORT);
    if(name == document.MemberEnd() || port == document.MemberEnd()) {
        return;
    }
    gate_server_name = name->value.GetString();
    gate_server_port = port->value.GetInt();

    butil::EndPoint ip_and_port = cntl->remote_side();
    //ip_and_port.port = gate_server_port;

    _session->record(gate_server_name, gate_server_port, ip_and_port.ip);

    LOG(INFO) << "Session had register: {" << gate_server_name << ":" << gate_server_port << "}";
}

void AuthServiceImpl::LastSend(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done)
{
    ::brpc::ClosureGuard done_guard(done);
    ::brpc::Controller *cntl = static_cast<::brpc::Controller*>(controller);

    std::string msg = request->message();
    _session->record(msg);
}

int AuthServiceImpl::init(ConfigParse *cParse, Session *session, Channel *channel) {
    _session = session;
    _accessRedis = new AccessRedis();
    if(_accessRedis->init(channel)) {
        LOG(ERROR) << "Fail to init AccessRedis";
        return 1;
    }
    return 0;
}

}    // namespace YTalk