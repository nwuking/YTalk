/*================================================================================   
 *    Date: 2021-12-27
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "IMService.h"
#include "base/Logging.h"
#include "base/ConfigParse.h"

namespace YTalk
{

IMServiceImpl::IMServiceImpl() {
    ///TODO
}

IMServiceImpl::~IMServiceImpl() {
    //TODO
}

int IMServiceImpl::init(ConfigParse *cParse) {
    /// TODO
}

void IMServiceImpl::Send(::google::protobuf::RpcController* controller,
                       const ::IMServer::Request* request,
                       ::IMServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    ///TODO
}

}      //// namespce YTalk