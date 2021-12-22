/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "ConLogin.h"
#include "base/Logging.h"
#include "base/ConfigParse.h"

#define LOGIN_SERVER_IP "login_server_ip"
#define LOGIN_SERVER_PORT "login_server_port"
#define CONNECTION_TYPE "connection_type"
#define TIME_OUT "time_out"
#define MAX_RETRY "max_retry"

namespace YTalk
{

ConLogin::ConLogin() {
    //TODO
}

ConLogin::~ConLogin() {
    //TODO
}

int ConLogin::init(ConfigParse *cParse) {
    if(!cParse) {
        LOG(ERROR) << "ConFigParse is nullptr";
        return 1;
    }

    std::string ip, port;
    cParse->getValue(LOGIN_SERVER_IP, ip);
    cParse->getValue(LOGIN_SERVER_PORT, port);
    if(ip.empty() || port.empty()) {
        LOG(ERROR) << "Need to configure LoginServer's IP or Port";
        return 2;
    }

    brpc::ChannelOptions options;

    std::string connection_type, time_out_str, max_retry_str;
    cParse->getValue(CONNECTION_TYPE, connection_type);
    cParse->getValue(TIME_OUT, time_out_str);
    cParse->getValue(MAX_RETRY, max_retry_str);

    options.connection_type = connection_type;
    //TODO
}

}    /// namespace YTalk