/*================================================================================   
 *    Date: 2021-12-27
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_IMSERVICE_H
#define YTALK_IMSERVICE_H

#include "protobuf/im.pb.h"
#include "brpc/server.h"

namespace YTalk
{

class ConfigParse;

class IMServiceImpl : public IMServer::IMService
{
public: 
    IMServiceImpl();
    virtual ~IMServiceImpl();

    int init(ConfigParse *cParse);

    virtual void Send(::google::protobuf::RpcController* controller,
                       const ::IMServer::Request* request,
                       ::IMServer::Response* response,
                       ::google::protobuf::Closure* done);

};   //// class IMServiceImpl


}   //// namespace TYalk

#endif
