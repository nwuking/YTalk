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

#define TOKEN "token"
#define USERNAME "username"

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
        response->set_status(GATE_STATUS_OK);
    }
    else {
        LOG(ERROR) << "Fail to send msg to RouteServer";
        response->set_status(GATE_STATUS_FAIL);
    }
}

void GateServiceImpl::FirstSend(::google::protobuf::RpcController* controller,
                       const ::GateServer::Request* request,
                       ::GateServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    ::brpc::ClosureGuard done_guard(done);
    ::brpc::Controller *cntl = static_cast<::brpc::Controller*>(controller);

    std::string msg = request->message();

    rapidjson::Document document;
    if(document.Parse(msg.c_str()).HasParseError()) {
        LOG(ERROR) << "Fail to parse msg";
        response->set_status(LOGIN_SERVER_ERROR);
        return;
    }

    rapidjson::Value::MemberIterator t = document.FindMember(TOKEN);
    rapidjson::Value::MemberIterator u = document.FindMember(USERNAME);
    if(t == document.MemberEnd() || u == document.MemberEnd()) {
        LOG(ERROR) << "Client ERROR";
        response->set_status(LOGIN_CLIENT_ERROR);
        return;
    }

    std::string token = t->value.GetString();
    std::string username = u->value.GetString();

    if(!_conLogin->auth(username, token)) {
        butil::EndPoint client_ip_and_port = cntl->remote_side();
        if(_channels->add(username, client_ip_and_port)) {
            response->set_status(LOGIN_FAIL);
        }
        else {
            response->set_status(LOGIN_SUCCESS);
        }        
    }
    else {
        response->set_status(LOGIN_FAIL);
    }
}

int GateServiceImpl::init(ConfigParse *cParse, ConLogin *cLogin, Channels *_cha, ConRoute *cRoute) {
    _conLogin = cLogin;
    _channels = _cha;
    _conRoute = cRoute;
}

}    /// namespace YTalk