/*================================================================================   
 *    Date: 2021-12-24
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CONROUTE_H
#define YTALK_CONROUTE_H

#include <string>
 
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
    std::string _route_server_ip;
    std::string _route_server_port_str;

};    //// class ConRoute

}   /// namespce YTalk

#endif