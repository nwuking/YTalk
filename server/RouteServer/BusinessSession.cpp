/*================================================================================   
 *    Date: 2021-12-26
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "BusinessSession.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"

#define CONNECTION_TYPE "connection_type_to_business"
#define TIME_OUT "time_out_to_business"
#define MAX_RETRY "max_retry_to_business"

namespace YTalk
{

BusinessSession::BusinessSession() {
    ///TODO
}

BusinessSession::~BusinessSession() {
    for(auto &p : _channel_map) {
        delete p.second;
        p.second = nullptr;
    }
    ///TODO
}

int BusinessSession::init(ConfigParse *cParse) {
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

void BusinessSession::record(const std::string &server_name, struct in_addr &server_ip, int server_port) {
    if(_channel_map.find(server_name) != _channel_map.end()) {
        LOG(ERROR) << "The BusinessServer:{" << server_name << "} had exist";
        return;
    }

    std::string server_ip_str = inet_ntoa(server_ip);
    std::string server_ip_and_port = server_ip_str + ":" + std::to_string(server_port);

    brpc::Channel *channel = new brpc::Channel();

    if(channel->Init(server_ip_and_port.c_str(), &_options) != 0) {
        LOG(ERROR) << "Fail to init Channel(to " << server_name << ")";
        return;
    }

    _channel_map.insert(std::make_pair(server_name, channel));
    _nameVec.push_back(server_name);
    ///TODO
}

}   //// namesapce YTalk