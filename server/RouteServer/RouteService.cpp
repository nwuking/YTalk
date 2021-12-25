/*================================================================================   
 *    Date: 2021-12-24
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "RouteService.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"
#include "rapidjson/document.h"
#include "brpc/server.h"
#include "GateSession.h"

#define GATE_SERVER_NAME "name"
#define GATE_SERVER_PORT "port"

namespace YTalk
{

RouteServiceImpl::RouteServiceImpl() {
    //TODO
}

RouteServiceImpl::~RouteServiceImpl() {
    //TODO
}

int RouteServiceImpl::init(ConfigParse *cParse, GateSession *gSession) {
    _gateSession = gSession;
    //TODO
    return 0;
}

void RouteServiceImpl::ToBusinessLayer(::google::protobuf::RpcController* controller,
                       const ::RouteServer::Request* request,
                       ::RouteServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    //TODO
}

void RouteServiceImpl::ToBottomLayer(::google::protobuf::RpcController* controller,
                       const ::RouteServer::Request* request,
                       ::RouteServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    //TODO
}

void RouteServiceImpl::FirstSend(::google::protobuf::RpcController* controller,
                       const ::RouteServer::Request* request,
                       ::RouteServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);

    std::string reqMsg = request->message();
    rapidjson::Document document;
    if(document.Parse(reqMsg.c_str()).HasParseError()) {
        LOG(ERROR) << "Fail to parse json: FirstSend";
        return;
    }

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

    _gateSession->record(gate_server_name, ip_and_port.ip, gate_server_port);

    LOG(INFO) << "GateSession had register: {" << gate_server_name << ":" << gate_server_port << "}";
    //TODO
}

}    //// namesapce YTalk