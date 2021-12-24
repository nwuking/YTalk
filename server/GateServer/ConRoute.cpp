/*================================================================================   
 *    Date: 2021-12-24
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "ConRoute.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"

#define ROUTE_SERVER_IP "route_server_ip"
#define ROUTE_SERVER_PORT "route_server_port"

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
    /// TODO
    return 0;
}

}    /// namesapce YTalk