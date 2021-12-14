/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/


#ifndef YTALK_LOGINSERVICE_H
#define YTALK_LOGINSERVICE_H

#include "protobuf/login.pb.h"

namespace YTalk
{

class AccessMySql;
class ConfigParse;

class LoginServiceImpl : public ::LoginServer::HttpService
{
public:
    LoginServiceImpl();
    virtual ~LoginServiceImpl();

    virtual void Login(::google::protobuf::RpcController* controller,
                       const ::LoginServer::HttpRequest* request,
                       ::LoginServer::HttpResponse* response,
                       ::google::protobuf::Closure* done);

    virtual void Register(::google::protobuf::RpcController* controller,
                       const ::LoginServer::HttpRequest* request,
                       ::LoginServer::HttpResponse* response,
                       ::google::protobuf::Closure* done);

    int init(ConfigParse *cParse);

private:
    AccessMySql *_accessMySql;

};    // class LoginServiceImpl

}    // namespace YTalk

#endif