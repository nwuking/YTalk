/*================================================================================   
 *    Date: 2021-12-24
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_ROUTESERVICE_H
#define YTALK_ROUTESERVICE_H

#include "protobuf/route.pb.h"

namespace YTalk
{

class ConfigParse;
class GateSession;

class RouteServiceImpl : public RouteServer::RouteService
{
public:
    RouteServiceImpl();
    virtual ~RouteServiceImpl();

    int init(ConfigParse *cParse, GateSession *gSession);

    virtual void ToBusinessLayer(::google::protobuf::RpcController* controller,
                       const ::RouteServer::Request* request,
                       ::RouteServer::Response* response,
                       ::google::protobuf::Closure* done);

    virtual void ToBottomLayer(::google::protobuf::RpcController* controller,
                       const ::RouteServer::Request* request,
                       ::RouteServer::Response* response,
                       ::google::protobuf::Closure* done);

    virtual void FirstSend(::google::protobuf::RpcController* controller,
                       const ::RouteServer::Request* request,
                       ::RouteServer::Response* response,
                       ::google::protobuf::Closure* done);

private:
    GateSession *_gateSession;
};    /// class 

}    // namespace YTalk

#endif