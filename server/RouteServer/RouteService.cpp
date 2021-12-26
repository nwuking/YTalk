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
#include "BusinessSession.h"
#include "base/structs.h"

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

int RouteServiceImpl::init(ConfigParse *cParse, GateSession *gSession, BusinessSession *bSession) {
    _gateSession = gSession;
    _businessSession = bSession;
    //TODO
    return 0;
}

void RouteServiceImpl::ToBusinessLayer(::google::protobuf::RpcController* controller,
                       const ::RouteServer::Request* request,
                       ::RouteServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);

    int32_t flag = request->flag();
    
    switch(flag) {
        case FLAG_IM:
            //TODO
            break;
        default:
            //TODO
    }
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

    std::string server_name;
    int server_port;
    rapidjson::Value::MemberIterator name = document.FindMember(GATE_SERVER_NAME);
    rapidjson::Value::MemberIterator port = document.FindMember(GATE_SERVER_PORT);
    if(name == document.MemberEnd() || port == document.MemberEnd()) {
        return;
    }
    server_name = name->value.GetString();
    server_port = port->value.GetInt();

    butil::EndPoint ip_and_port = cntl->remote_side();

    int32_t flag = request->flag();

    switch(flag) {
        case FLAG_GATE_SERVER:
            _gateSession->record(server_name, ip_and_port.ip, server_port);
            LOG(INFO) << "GateSession had register: {" << server_name << ":" << server_port << "}";
            break;

        case FLAG_IM_SERVER:
            _businessSession->record(server_name, ip_and_port.ip, server_port);
            LOG(INFO) << "BusinessSession had register: {" << server_name << ":" << server_port << "}";
            break;

        default:
            break;
    //TODO
    }
}

}    //// namesapce YTalk