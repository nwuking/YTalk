/*================================================================================   
 *    Date: 2021-12-25
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "GateSession.h"
#include "base/Logging.h"
#include "base/ConfigParse.h"

#define CONNECTION_TYPE "connection_type_to_gate"
#define TIME_OUT "timeout_ms_to_gate"
#define MAX_RETRY "max_retry_to_gate"

namespace YTalk
{

GateSession::GateSession() {
    //TODO
}

GateSession::~GateSession() {
    for(auto &p : _gate_channel) {
        delete p.second;
        p.second = nullptr;
    }
    //TODO
}

int GateSession::init(ConfigParse *cParse) {
    if(!cParse) {
        return 0;
    }

    std::string connection_type, max_retry_str, time_out_str;
    cParse->getValue(CONNECTION_TYPE, connection_type);
    cParse->getValue(TIME_OUT, time_out_str);
    cParse->getValue(MAX_RETRY, max_retry_str);

    _options.connection_type = connection_type;
    if(!max_retry_str.empty()) {
        _options.timeout_ms = std::stoi(time_out_str);
    }
    if(!max_retry_str.empty()) {
        _options.max_retry = std::stoi(max_retry_str);
    }

    return 0;
}

void GateSession::record(const std::string &server_name, struct in_addr &server_ip, int server_port) {
    if(_gate_channel.find(server_name) != _gate_channel.end()) {
        LOG(ERROR) << "The GateServer:{" << server_name << "} had exist";
        return;
    }

    std::string server_ip_str = inet_ntoa(server_ip);
    std::string server_ip_and_port = server_ip_str + ":" + std::to_string(server_port);

    brpc::Channel *channel = new brpc::Channel();

    if(channel->Init(server_ip_and_port.c_str(), &_options) != 0) {
        LOG(ERROR) << "Fail to init Channel(to " << server_name << ")";
        return;
    }

    _gate_channel.insert(std::make_pair(server_name, channel));
}

} // namespace YTalk

///TODO
