/*================================================================================   
 *    Date: 2021-12-23
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_CHANNELS_H
#define YTALK_CHANNELS_H

#include <unordered_map>

#include "brpc/channel.h"
#include "base/Mutex.h"

namespace YTalk
{

class ConfigParse;

class Channels
{
public:
    Channels();
    virtual ~Channels();

    int init(ConfigParse *cParse);

    int add(const std::string &username, butil::EndPoint &ep);

private:
    std::unordered_map<std::string, brpc::Channel*> _channel_map;

    brpc::ChannelOptions _options;
    Mutex _mutex;

};   /// class Channels

}    /// namesapce YTalk

#endif