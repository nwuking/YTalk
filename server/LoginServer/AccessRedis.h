/*================================================================================   
 *    Date: 2022-2-3
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_ACCESSREDIS_H
#define YTALK_ACCESSREDIS_H

#include <string>

#include "brpc/channel.h"

namespace YTalk
{

//class ConfigParse;
class Channel;

class AccessRedis
{
public:
    AccessRedis();
    virtual ~AccessRedis();

    int init(Channel *channel);

    int queryForOnline(const std::string &u_name);

    int queryForToken(const std::string &u_name, std::string &token);

    int setToken(const std::string &u_name, const std::string &token);

    int setOnlineStatus(const std::string &u_name, bool flag = true);


private:
    Channel *_channel;

};   /// class AccessRedis

}   /// namespace YTalk

#endif