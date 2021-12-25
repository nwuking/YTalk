/*================================================================================   
 *    Date: 2021-12-24
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "ConRoute.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"
#include "RouteServer/protobuf/route.pb.h"

#define ROUTE_SERVER_IP "route_server_ip"
#define ROUTE_SERVER_PORT "route_server_port"
#define GATE_SERVER_NAME "gate_server_name"
#define GATE_SERVER_PORT "gate_server_port"
#define CONNECTION_TYPE "channel_option_connection_type"
#define TIME_OUT "channel_option_time_out"
#define MAX_RETRY "channel_option_max_retry"

namespace YTalk
{

ConRoute::ConRoute() {
    //TODO
}

ConRoute::~ConRoute() {
    ///TODO
}

int ConRoute::init(ConfigParse *cPasre) {
    if(!cPasre) {
        LOG(ERROR) << "ConfigParse is nullptr";
        return 1;
    }

    cPasre->getValue(ROUTE_SERVER_IP, _route_server_ip);
    cPasre->getValue(ROUTE_SERVER_PORT, _route_server_port_str);
    if(_route_server_port_str.empty() || _route_server_ip.empty()) {
        LOG(ERROR) << "You need to configure ip or port";
        return 2;
    }
    std::string server_ip_and_port = _route_server_ip + ":" + _route_server_port_str;

    cPasre->getValue(GATE_SERVER_NAME, _gate_server_name);
    cPasre->getValue(GATE_SERVER_PORT, _gate_server_port);
    if(_gate_server_name.empty() || _gate_server_port.empty()) {
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

    return 0;
}

bool ConRoute::firstSend() {
    RouteServer::RouteService_Stub stub(&_channel);
    RouteServer::Request request;
    RouteServer::Response response;
    brpc::Controller cntl;

    std::string msg = "{"
                            "\"name\" : \"" + _gate_server_name + "\", "
                            "\"port\" : " + _gate_server_port +
                       "}";

    request.set_message(msg);

    stub.FirstSend(&cntl, &request, &response, nullptr);
    if(cntl.Failed()) {
        LOG(ERROR) << "Fail to firsend: " << cntl.ErrorText();
        return false;
    }
   
    LOG(INFO) << _gate_server_name << ": Register successful in LoginServer";
    return true;
}

}    /// namesapce YTalk