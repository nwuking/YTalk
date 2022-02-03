/*================================================================================   
 *    Date: 2021-12-18
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
class Channel;

class AccessMySql
{
public:
    AccessMySql();
    virtual ~AccessMySql();

    int init(Channel *channel);

    int queryForLogin(const std::string &username, const std::string &password);

    //int updateForRegister(const std::string &username, const std::string &password);
    //int updateForRegister(const UserRegisterInfo &register_info);
    NewUserInfo* updateForRegister(const UserRegisterInfo &userRegisterInfo);

    int getMaxUserId();

private:
    Channel *_channel;
};    // class AccessMySql

}    // namesapce YTalk

#endif