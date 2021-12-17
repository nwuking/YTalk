/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "LoginService.h"
#include "base/ConfigParse.h"
#include "AccessMySql.h"
#include "base/Logging.h"

namespace YTalk
{

LoginServiceImpl::LoginServiceImpl() {
    _accessMySql == nullptr;
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
    //TODO
}

void LoginServiceImpl::Register(::google::protobuf::RpcController* controller,
                       const ::LoginServer::HttpRequest* request,
                       ::LoginServer::HttpResponse* response,
                       ::google::protobuf::Closure* done)
{
    //TODO
}

int LoginServiceImpl::init(ConfigParse *cParse) {
    _accessMySql = new AccessMySql();
    if(!_accessMySql) {
        LOG(ERROR) << "new AccessMySql Fail";
        return 1;
    }
    _accessMySql->init(cParse);
    //TODO
}

}    //   namespace YTalk