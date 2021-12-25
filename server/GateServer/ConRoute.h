/*================================================================================   
 *    Date: 2021-12-24
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CONROUTE_H
#define YTALK_CONROUTE_H

#include <string>

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

    bool firstSend();

private:
    std::string _route_server_ip;
    std::string _route_server_port_str;

    std::string _gate_server_name;
    std::string _gate_server_port;

    brpc::Channel _channel;

};    //// class ConRoute

}   /// namespce YTalk

#endif