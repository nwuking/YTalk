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

class GateSession
{
public:
    GateSession();
    virtual ~GateSession();

    void record(const std::string &server_name, struct in_addr &server_ip, int server_port);

private:
    std::unordered_map<std::string, brpc::Channel*> _gate_channel;
};     //// class Session

}   /// namespace YTalk

#endif