/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_ACCESSMYSQL_H
#define YTALK_ACCESSMYSQL_H

#include <string>

#include <brpc/channel.h>

namespace YTalk
{

class ConfigParse;

class AccessMySql
{
public:
    AccessMySql();
    virtual ~AccessMySql();

    int init(ConfigParse *cParse);

private:
    std::string _dbProxyServer_ip;
    std::string _dbProxyServer_port_str;

    brpc::Channel _channel;
};    // class AccessMySql

}    // namesapce YTalk

#endif