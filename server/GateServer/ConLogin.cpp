/*================================================================================   
 *    Date: 2021-12-23
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "ConLogin.h"
#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "LoginServer/protobuf/auth.pb.h"

#define LOGIN_SERVER_IP "login_server_ip"
#define LOGIN_SERVER_PORT "login_server_port"
#define CONNECTION_TYPE "connection_type"
#define TIME_OUT "time_out"
#define MAX_RETRY "max_retry"
#define GATE_SERVER_NAME "gate_server_name"
#define GATE_SERVER_PORT "gate_server_listen_port"

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
    std::string login_server_ip_and_port = ip + ":" + port;
    _login_server_ip = ip;
    _login_server_port = port;

    cParse->getValue(GATE_SERVER_NAME, _gate_server_name);
    if(_gate_server_name.empty()) {
        LOG(ERROR) << "You need to configure gate_server_name";
        return 3;
    }

    cParse->getValue(GATE_SERVER_PORT, _gate_server_port);
    if(_gate_server_port.empty()) {
        LOG(ERROR) << "You need to configure gate_server_listen_port";
        return 4;
    }

    brpc::ChannelOptions options;

    std::string connection_type, time_out_str, max_retry_str;
    cParse->getValue(CONNECTION_TYPE, connection_type);
    cParse->getValue(TIME_OUT, time_out_str);
    cParse->getValue(MAX_RETRY, max_retry_str);

    options.connection_type = connection_type;
    if(!time_out_str.empty()) {
        options.timeout_ms = std::stoi(time_out_str);
    }
    if(!max_retry_str.empty()) {
        options.max_retry = std::stoi(max_retry_str);
    }

    if(_channel.Init(login_server_ip_and_port.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to init channel";
        return 3;
    }
    
    firstSend();
    return 0;
}

void ConLogin::firstSend() {
    LoginServer::AuthService_Stub stub(&_channel);
    LoginServer::AuthRequest request;
    LoginServer::AuthResponse response;
    brpc::Controller cntl;

    std::string msg = "{"
                            "\"name\" : \"" + _gate_server_name + "\", "
                            "\"port\" : " + _gate_server_port +
                       "}";

    request.set_message(msg);

    stub.FirstSend(&cntl, &request, &response, nullptr);
    if(cntl.Failed()) {
        LOG(ERROR) << "Fail to firsend: " << cntl.ErrorText();
    }
    else {
        LOG(INFO) << _gate_server_name << ": Register successful in LoginServer";
    }
}

}    /// namespace YTalk