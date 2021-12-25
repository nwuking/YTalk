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

class GateServiceImpl : public GateServer::GateService
{
public:
    GateServiceImpl();
    virtual ~GateServiceImpl();

    virtual void Send(::google::protobuf::RpcController* controller,
                       const ::GateServer::Request* request,
                       ::GateServer::Response* response,
                       ::google::protobuf::Closure* done);

    virtual void FirstSend(::google::protobuf::RpcController* controller,
                       const ::GateServer::Request* request,
                       ::GateServer::Response* response,
                       ::google::protobuf::Closure* done);

    int init(ConfigParse *cParse, ConLogin *cLogin, Channels *cha);

private:
    ConLogin *_conLogin;
    Channels *_channels;
};    /// class 


}    // namespace YTalk

#endif
