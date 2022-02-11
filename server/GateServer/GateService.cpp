/*================================================================================   
 *    Date: 2021-12-25
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "GateService.h"
#include "rapidjson/document.h"
#include "base/Logging.h"
#include "base/structs.h"
#include "ConLogin.h"
#include "Channels.h"
#include "ConRoute.h"


namespace YTalk
{

GateServiceImpl::GateServiceImpl() {
    //TODO
}

GateServiceImpl::~GateServiceImpl() {
    //TODO
}

void GateServiceImpl::Send2Route(::google::protobuf::RpcController* controller,
                       const ::GateServer::Request* request,
                       ::GateServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);

    GateConText gateConText;
    gateConText.flag = request->flag();
    gateConText.msg = request->message();

    bool op = _conRoute->send2Route(gateConText);

    if(op) {
        LOG(INFO) << "Send 2 RouteServer sucessful";
        response->set_status(GATE_OK);
    }
    else {
        LOG(ERROR) << "Fail to send msg to RouteServer";
        response->set_status(GATE_FAIL);
    }
}

void GateServiceImpl::Login2Gate(::google::protobuf::RpcController* controller,
                       const ::GateServer::Request* request,
                       ::GateServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    ::brpc::ClosureGuard done_guard(done);
    ::brpc::Controller *cntl = static_cast<::brpc::Controller*>(controller);

    std::string msg = request->message();
/*
{
    "u_name": *****,
    "token": *******
}
*/
    rapidjson::Document document;
    if(document.Parse(msg.c_str()).HasParseError()) {
        LOG(ERROR) << "Fail to parse msg";
        response->set_status(GATE_FAIL);
        return;
    }

    rapidjson::Value::MemberIterator t = document.FindMember(U_TOKEN);
    rapidjson::Value::MemberIterator u = document.FindMember(U_NAME);
    if(t == document.MemberEnd() || u == document.MemberEnd()) {
        LOG(ERROR) << "Client ERROR";
        response->set_status(GATE_CLIENT_FAIL);
        return;
    }

    std::string token = t->value.GetString();
    std::string u_name = u->value.GetString();

    if(!_conLogin->auth(u_name, token)) {
        butil::EndPoint client_ip_and_port = cntl->remote_side();
        if(_channels->add(u_name, client_ip_and_port)) {
            response->set_status(GATE_FAIL);
        }
        else {
            response->set_status(GATE_OK);
        }        
    }
    else {
        response->set_status(GATE_FAIL);
    }
}

int GateServiceImpl::init(ConfigParse *cParse, ConLogin *cLogin, Channels *_cha, ConRoute *cRoute) {
    _conLogin = cLogin;
    _channels = _cha;
    _conRoute = cRoute;
}

}    /// namespace YTalk