/*================================================================================   
 *    Date: 2022-2-3
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "Channel.h"
#include "base/ConfigParse.h"
#include "base/structs.h"

namespace YTalk
{

Channel::Channel() {
    //TODO
}

Channel::~Channel() {
    //TODO
}

int Channel::init(ConfigParse *cParse) {
    if(!cParse->status()) {
        return 1;
    }

    cParse->getValue(DB_PROXY_SERVER_IP, _dbProxyServer_ip);
    cParse->getValue(DB_PROXY_SERVER_PORT, _dbProxyServer_port_str);
    if(_dbProxyServer_port_str.empty() || _dbProxyServer_ip.empty()) {
        LOG(ERROR) << "Not configure ip or port of DB_Proxy_server";
        return 2;
    }
    std::string server_ip_and_port = _dbProxyServer_ip + ":" + _dbProxyServer_port_str;

    brpc::ChannelOptions options;

    std::string max_retry_str;
    cParse->getValue(MAX_RETRY, max_retry_str);
    if(!max_retry_str.empty()) {
        options.max_retry = std::stoi(max_retry_str);
    } 

    std::string timeout_ms_str;
    cParse->getValue(TIMEOUT_MS, timeout_ms_str);
    if(!timeout_ms_str.empty()) {
        options.timeout_ms = std::stoi(timeout_ms_str);
    }

    std::string connection_type;
    cParse->getValue(CONNECTION_TYPE, connection_type);
    if(!connection_type.empty()) {
        options.connection_type = connection_type;
    }

    if(_channel.Init(server_ip_and_port.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initalize";
        return 3;
    }
    return 0;
}

}  /// namesapce YTalk