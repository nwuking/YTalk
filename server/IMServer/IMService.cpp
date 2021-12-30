/*================================================================================   
 *    Date: 2021-12-27
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "IMService.h"
#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "base/structs.h"
#include "ConRoute_im.h"

namespace YTalk
{

IMServiceImpl::IMServiceImpl() {
    ///TODO
}

IMServiceImpl::~IMServiceImpl() {
    //TODO
}

int IMServiceImpl::init(ConfigParse *cParse, ConRoute *cRoute) {
    /// TODO
    return 0;
}

void IMServiceImpl::Send(::google::protobuf::RpcController* controller,
                       const ::IMServer::Request* request,
                       ::IMServer::Response* response,
                       ::google::protobuf::Closure* done)
{
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = static_cast<brpc::Controller*>(controller);
    ///TODO
    response->set_status(IM_STATUS_OK);
}

}      //// namespce YTalk