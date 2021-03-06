/*================================================================================   
 *    Date: 2021-12-25
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_GATESESSION_H
#define YTALK_GATESESSION_H

#include <unordered_map>
#include <string>
#include <arpa/inet.h>

#include "brpc/channel.h"

namespace YTalk
{

class ConfigParse;

class GateSession
{
public:
    GateSession();
    virtual ~GateSession();

    int init(ConfigParse *cParse = nullptr);

    void record(const std::string &server_name, struct in_addr &server_ip, int server_port);

private:
    std::unordered_map<std::string, brpc::Channel*> _gate_channel;
    brpc::ChannelOptions _options;
};     //// class Session

}   /// namespace YTalk

#endif