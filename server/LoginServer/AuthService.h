/*================================================================================   
 *    Date: 2021-12-20
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_AUTHSERVICE_H
#define YTALK_AUTHSERVICE_H

#include "protobuf/auth.pb.h"

namespace YTalk
{

class ConfigParse;
class Session;
class Channel;
class AccessRedis;

class AuthServiceImpl : public ::LoginServer::AuthService
{
public:
    AuthServiceImpl();
    ~AuthServiceImpl();

    virtual void Auth(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done);

    virtual void Notify(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done);

    virtual void FirstSend(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done);

    virtual void LastSend(::google::protobuf::RpcController* controller,
                       const ::LoginServer::AuthRequest* request,
                       ::LoginServer::AuthResponse* response,
                       ::google::protobuf::Closure* done);

    int init(ConfigParse *cParse, Session *session, Channel *channel);

private:
    Session *_session;
    AccessRedis *_accessRedis;

};     ///  class AuthServiceImpl

}    // namespace YTalk

#endif