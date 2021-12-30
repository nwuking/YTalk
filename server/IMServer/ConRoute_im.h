/*================================================================================   
 *    Date: 2021-12-30
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CONROUTE_IM_H
#define YTALK_CONROUTE_IM_H

#include "brpc/channel.h"

namespace YTalk
{

class ConfigParse;

class ConRoute
{
public:
    ConRoute();
    virtual ~ConRoute();

    int init(ConfigParse *cParse);

private:
    bool firstSend();

    std::string _route_server_ip;
    std::string _route_server_port_str;

    std::string _im_server_name;
    std::string _im_server_port;

    brpc::Channel _channel;
};

}   /// namespace YTalk

#endif