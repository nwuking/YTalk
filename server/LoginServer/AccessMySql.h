/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_ACCESSMYSQL_H
#define YTALK_ACCESSMYSQL_H

#include <string>

#include <brpc/channel.h>

#include "base/structs.h"

namespace YTalk
{

class ConfigParse;

class AccessMySql
{
public:
    AccessMySql();
    virtual ~AccessMySql();

    int init(ConfigParse *cParse);

    int queryForLogin(const std::string &username, const std::string &password);

    //int updateForRegister(const std::string &username, const std::string &password);
    int updateForRegister(const UserRegisterInfo &register_info);

private:
    std::string _dbProxyServer_ip;
    std::string _dbProxyServer_port_str;

    brpc::Channel _channel;
};    // class AccessMySql

}    // namesapce YTalk

#endif