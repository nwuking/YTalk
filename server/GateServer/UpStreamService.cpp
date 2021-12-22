/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "UpStreamService.h"
#include "rapidjson/document.h"
#include "base/Logging.h"
#include "base/structs.h"

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
    //TODO
}

}    /// namespace YTalk