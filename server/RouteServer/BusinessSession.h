/*================================================================================   
 *    Date: 2021-12-26
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_BUSINESSSESSION_H
#define YTALK_BUSINESSSESSION_H

#include <unordered_map>
#include <string>
#include <vector>
#include <arpa/inet.h>

#include "brpc/channel.h"

namespace YTalk
{

class ConfigParse;

class BusinessSession
{
public:
    BusinessSession();
    virtual ~BusinessSession();

    int init(ConfigParse *cParse);

    void record(const std::string &server_name, struct in_addr &server_ip, int server_port);

    bool send();

private:
    std::vector<std::string> _nameVec;
    std::unordered_map<std::string, brpc::Channel*> _channel_map;

    brpc::ChannelOptions _options;

};   /// class BusinessSession

} ////// namepace YTalk

#endif