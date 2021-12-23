/*================================================================================   
 *    Date: 2021-12-23
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CONLOGIN_H
#define YTALK_CONLOGIN_H

#include "brpc/channel.h"

namespace YTalk
{

class ConfigParse;

class ConLogin
{
public:
    ConLogin();
    virtual ~ConLogin();

    int init(ConfigParse *cParse);

    int auth(const std::string &username, const std::string &token);

    void firstSend();

private:
    brpc::Channel _channel;

    std::string _gate_server_name;
    std::string _gate_server_port;
    
    std::string _login_server_ip;
    std::string _login_server_port;

    //static ConLogin *_conLogin;
};    /// class ConLogin

}   //// namesapce YTalk

#endif