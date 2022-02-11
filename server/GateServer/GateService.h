/*================================================================================   
 *    Date: 2021-12-25
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_GATESERVICE_H
#define YTALK_GATESERVICE_H

#include "protobuf/gate.pb.h"

namespace YTalk
{

class ConfigParse;
class ConLogin;
class Channels;
class ConRoute;

class GateServiceImpl : public GateServer::GateService
{
public:
    GateServiceImpl();
    virtual ~GateServiceImpl();

    virtual void Send2Route(::google::protobuf::RpcController* controller,
                       const ::GateServer::Request* request,
                       ::GateServer::Response* response,
                       ::google::protobuf::Closure* done);

    virtual void Login2Gate(::google::protobuf::RpcController* controller,
                       const ::GateServer::Request* request,
                       ::GateServer::Response* response,
                       ::google::protobuf::Closure* done);

    int init(ConfigParse *cParse, ConLogin *cLogin, Channels *cha, ConRoute *cRoute);

private:
    ConLogin *_conLogin;
    Channels *_channels;
    ConRoute *_conRoute;
};    /// class 


}    // namespace YTalk

#endif
