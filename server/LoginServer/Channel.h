/*================================================================================   
 *    Date: 2022-2-3
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CHANNEL_H
#define YTALK_CHANNEL_H

#include "brpc/channel.h"
#include "base/ConfigParse.h"

#include <string>

namespace YTalk
{

class ConfigParse;

class Channel
{
public:
    Channel();
    virtual ~Channel();

    int init(ConfigParse *cParse);

    bool getStatus() {
        return _status;
    }

    brpc::Channel* getChannel() {
        return &_channel;
    }

private:
    std::string _dbProxyServer_ip;
    std::string _dbProxyServer_port_str;

    brpc::Channel _channel;
    bool _status;
};

}   /// namespace YTalk

#endif