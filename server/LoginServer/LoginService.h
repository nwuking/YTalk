/*================================================================================   
 *    Date: 2021-12-21
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/


#ifndef YTALK_LOGINSERVICE_H
#define YTALK_LOGINSERVICE_H

#include <atomic>

#include "protobuf/login.pb.h"
#include "base/Mutex.h"

namespace YTalk
{

class AccessMySql;
class AccessRedis;
class ConfigParse;
class Session;
class Channel;

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

    int init(ConfigParse *cParse, Session *session, Channel *channel);

private:
    AccessMySql *_accessMySql;
    AccessRedis *_accessRedis;
    Session *_session;

    std::atomic_int _baseUserId{0};

    Mutex _mutex;

};    // class LoginServiceImpl

}    // namespace YTalk

#endif