/*================================================================================   
 *    Date: 2021-12-22
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_UPSTREAMSERVICE_H
#define YTALK_UPSTREAMSERVICE_H

#include "protobuf/upstream.pb.h"


namespace YTalk
{

class ConfigParse;
class ConLogin;
class Channels;

class UpStreamServiceImpl : public GateServer::UpStreamService
{
public:
    UpStreamServiceImpl();
    virtual ~UpStreamServiceImpl();

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