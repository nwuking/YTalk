/*================================================================================   
 *    Date: 2021-12-30
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "ConRoute_im.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"
#include "RouteServer/protobuf/route.pb.h"
#include "base/structs.h"

#define ROUTE_SERVER_IP "route_server_ip"
#define ROUTE_SERVER_PORT "route_server_port"
#define CONNECTION_TYPE "channel_option_connection_type"
#define TIME_OUT "channel_option_time_out"
#define MAX_RETRY "channel_option_max_retry"
#define IM_SERVER_NAME "im_server_name"
#define IM_SERVER_PORT "im_server_listen_port"

namespace YTalk
{

ConRoute::ConRoute() {
    ///TODO
}

ConRoute::~ConRoute() {
    //TODO
}

int ConRoute::init(ConfigParse *cParse) {
    if(!cParse) {
        LOG(ERROR) << "ConfigParse is nullptr";
        return 1;
    }

    cParse->getValue(ROUTE_SERVER_IP, _route_server_ip);
    cParse->getValue(ROUTE_SERVER_PORT, _route_server_port_str);
    if(_route_server_port_str.empty() || _route_server_ip.empty()) {
        LOG(ERROR) << "You need to configure ip or port";
        return 2;
    }
    std::string server_ip_and_port = _route_server_ip + ":" + _route_server_port_str;

    cParse->getValue(IM_SERVER_NAME, _im_server_name);
    cParse->getValue(IM_SERVER_PORT, _im_server_port);
    if(_im_server_name.empty() || _im_server_port.empty()) {
        LOG(ERROR) << "You need to configure GateServer name or port";
        return 3;
    }

    brpc::ChannelOptions options;

    std::string connection_type, time_out_str, max_retry_str;
    cParse->getValue(CONNECTION_TYPE, connection_type);
    cParse->getValue(TIME_OUT, time_out_str);
    cParse->getValue(MAX_RETRY, max_retry_str);

    options.connection_type = connection_type;
    if(!time_out_str.empty()) {
        options.timeout_ms = std::stoi(time_out_str);
    }
    if(!max_retry_str.empty()) {
        options.max_retry = std::stoi(max_retry_str);
    } 

    if(_channel.Init(server_ip_and_port.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to init Channel";
        return 4;
    }

    if(!firstSend()) {
        LOG(ERROR) << "Fail to firstSend to RouteServer";
        return 5;
    }
    //TODO
    return 0;
}

bool ConRoute::firstSend() {
    RouteServer::RouteService_Stub stub(&_channel);
    RouteServer::Request request;
    RouteServer::Response response;
    brpc::Controller cntl;

    std::string msg = "{"
                            "\"name\" : \"" + _im_server_name + "\", "
                            "\"port\" : " + _im_server_port +
                       "}";

    request.set_message(msg);
    request.set_flag(FLAG_IM_SERVER);

    stub.FirstSend(&cntl, &request, &response, nullptr);
    if(cntl.Failed()) {
        LOG(ERROR) << "Fail to firsend: " << cntl.ErrorText();
        return false;
    }
   
    LOG(INFO) << _im_server_name << ": Register successful in LoginServer";
    return true;
}

}   //// namespce YTalk