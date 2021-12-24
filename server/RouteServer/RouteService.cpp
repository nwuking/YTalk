/*================================================================================   
 *    Date: 2021-12-24
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "RouteService.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"

namespace YTalk
{

RouteServiceImpl::RouteServiceImpl() {
    //TODO
}

RouteServiceImpl::~RouteServiceImpl() {
    //TODO
}

int RouteServiceImpl::init(ConfigParse *cParse) {
    //TODO
    return 0;
}

void RouteServiceImpl::ToBusinessLayer(::google::protobuf::RpcController* controller,
                       const ::RouteServer::Request* request,
                       ::RouteServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    //TODO
}

void RouteServiceImpl::ToBottomLayer(::google::protobuf::RpcController* controller,
                       const ::RouteServer::Request* request,
                       ::RouteServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    //TODO
}

}    //// namesapce YTalk