/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "AccessMySql.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"

#define DB_PROXY_SERVER_IP "db_proxy_server_ip"
#define DB_PROXY_SERVER_PORT "db_proxy_server_port"
#define MAX_RETRY "max_retry"

namespace YTalk
{

AccessMySql::AccessMySql() {
    //TODO
}

AccessMySql::~AccessMySql() {
    //TODO
}

int AccessMySql::init(ConfigParse *cParse) {
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
    //TODO
    if(_channel.Init(server_ip_and_port.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initalize";
        return 3;
    }
    return 0;
}

}    // namesapce YTalk