/*================================================================================   
 *    Date: 2021-12-24
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CONROUTE_H
#define YTALK_CONROUTE_H

#include <string>

#include "brpc/channel.h"
#include "base/structs.h"
 
namespace YTalk
{

class ConfigParse;

class ConRoute
{
public:
    ConRoute();
    virtual ~ConRoute();

    int init(ConfigParse *cParse);

    bool send2Route(const GateConText &cnt);

    //bool firstSend();

private:
    bool firstSend();

    std::string _route_server_ip;
    std::string _route_server_port_str;

    std::string _gate_server_name;
    std::string _gate_server_port;

    brpc::Channel _channel;

};    //// class ConRoute

}   /// namespce YTalk

#endif