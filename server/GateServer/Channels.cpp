/*================================================================================   
 *    Date: 2021-12-23
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "Channels.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"

#define CHANNEL_OPTION_CONNECTION_TYPE "channel_option_connection_type"
#define CHANNEL_OPTION_TIME_OUT "channel_option_time_out"
#define CHANNEL_OPTION_MAX_RETRY "channel_option_max_retry"

namespace YTalk
{

Channels::Channels() {
    //TODO
}

Channels::~Channels() {
    for(auto &p : _channel_map) {
        delete p.second;
        p.second = nullptr;
    }
    //TODO
}

int Channels::init(ConfigParse *cParse) {
    if(!cParse) {
        LOG(ERROR) << "ConfigParse is nullptr";
        return 1;
    }

    std::string connection_type, time_out_str, max_retry_str;
    cParse->getValue(CHANNEL_OPTION_CONNECTION_TYPE, connection_type);
    cParse->getValue(CHANNEL_OPTION_TIME_OUT, time_out_str);
    cParse->getValue(CHANNEL_OPTION_MAX_RETRY, max_retry_str);
    if(!time_out_str.empty()) {
        _options.timeout_ms = std::stoi(time_out_str);
    }
    if(!max_retry_str.empty()) {
        _options.max_retry = std::stoi(max_retry_str);
    }
    _options.connection_type = connection_type;
    //TODO
}

int Channels::add(const std::string &username, butil::EndPoint &ep) {
    brpc::Channel *channel = new brpc::Channel();
    if(channel->Init(ep, &_options) != 0) {
        LOG(ERROR) << "Fail to Init brpc::channel: " << username;
        return 1;
    }

    MutexLock lock(_mutex);
    _channel_map.insert(std::make_pair(username, channel));
    return 0;
}

}    /// namespace YTalk