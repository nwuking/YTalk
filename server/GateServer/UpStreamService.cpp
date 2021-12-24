/*================================================================================   
 *    Date: 2021-12-22
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "UpStreamService.h"
#include "rapidjson/document.h"
#include "base/Logging.h"
#include "base/structs.h"
#include "ConLogin.h"
#include "Channels.h"

#define TOKEN "token"
#define USERNAME "username"

namespace YTalk
{

UpStreamServiceImpl::UpStreamServiceImpl() {
    //TODO
}

UpStreamServiceImpl::~UpStreamServiceImpl() {
    //TODO
}

void UpStreamServiceImpl::Send(::google::protobuf::RpcController* controller,
                       const ::GateServer::Request* request,
                       ::GateServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);
    //TODO
}

void UpStreamServiceImpl::FirstSend(::google::protobuf::RpcController* controller,
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

int UpStreamServiceImpl::init(ConfigParse *cParse, ConLogin *cLogin, Channels *_cha) {
    _conLogin = cLogin;
    _channels = _cha;
}

}    /// namespace YTalk