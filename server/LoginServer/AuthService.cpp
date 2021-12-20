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

#include "brpc/server.h"
#include "rapidjson/document.h"

#define GATE_SERVER_NAME "name"
#define GATE_SERVER_PORT "port"

namespace YTalk
{

AuthServiceImpl::AuthServiceImpl() {
    //TODO
}

AuthServiceImpl::~AuthServiceImpl() {
    //TODO
}

void AuthServiceImpl::Auth(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done)
{
    //TODO
}

void AuthServiceImpl::Notify(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done)
{
    //TODO
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

    std::string gate_server_name, gate_server_port;
    rapidjson::Value::MemberIterator name = document.FindMember(GATE_SERVER_NAME);
    rapidjson::Value::MemberIterator port = document.FindMember(GATE_SERVER_PORT);
    if(name == document.EndMember() || port == document.EndMember()) {
        return;
    }
    gate_server_name = name->value.GetString();
    gate_server_port = name->value.GetString();

    butil::EndPoint ip_and_port = cntl->remote_side();

    _session->record(gate_server_name, gate_server_port, ip_and_port.ip);
}

void AuthServiceImpl::LastSend(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done)
{
    //TODO
}

int AuthServiceImpl::init(ConfigParse *cParse, Session *session) {
    _session = session;
    //TODO
}

}    // namespace YTalk