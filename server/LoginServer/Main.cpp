/*================================================================================   
 *    Date: 2021-12-22
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <brpc/server.h>
#include <gflags/gflags.h>

#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "Session.h"
#include "LoginService.h"
#include "AuthService.h"
#include "Channel.h"
#include "Token.h"

DEFINE_string(ip, "login_server_listen_ip", "login server ip key");
DEFINE_string(client_port, "client_listen_port", "for listen client");
DEFINE_string(im_gate_port, "im_gate_listen_port", "for listen IMGateServre");
DEFINE_string(config, "./login_server.conf", "cnfig file");
DEFINE_string(log_config, "./login_server_log.conf", "log configure");
DEFINE_string(threads, "num_threads", "the number of working threads");
DEFINE_string(server_max_concurrency, "server_max_concurrency", "sever-level");

int main(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    signal(SIGPIPE, SIG_IGN);

    /// for LOG
    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance(/*FLAGS_log_config.c_str()*/);
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);

    ::YTalk::ConfigParse confParse;
    confParse.parse(FLAGS_config);
    if(!confParse.status()) {
        LOG(ERROR) << "Fail to parse config";
        return 1;
    }

    ::YTalk::Session *session = YTalk::Session::getInstance();
    YTalk::Channel channel;
    YTalk::Token token;

    if(token.init()) {
        LOG(ERROR) << "Fail to init Token";
        return 1;
    }

    if(channel.init(&confParse)) {
        LOG(ERROR) << "Fail to init Channel";
        return 1;
    }
    
    ::YTalk::LoginServiceImpl loginService;
    if(loginService.init(&confParse, session, &channel, &token)) {
        LOG(ERROR) << "Fail to init LoginServiceImpl";
        return 2;
    }
    LOG(INFO) << "Initializate LoginServiceImpl";

    ::YTalk::AuthServiceImpl authService;
    if(authService.init(&confParse, session)) {
        LOG(ERROR) << "Fail to init AuthServiceImpl";
        return 3;
    }
    LOG(INFO) << "Initializate AuthServiceImpl";
/*
    /// for test
    brpc::Controller cntl;
    std::string msg = "{\"name\" : \"nwuk\", \"port\" : 110}";
    LoginServer::AuthRequest request;
    request.set_message(msg.c_str());
    authService.FirstSend(&cntl, &request, nullptr, nullptr);
    //// end
*/
    brpc::Server http_server, server;

    if(http_server.AddService(&loginService, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add LoginServiceImpl";
        return 4;
    }
    if(server.AddService(&authService, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add AuthServiceImpl";
        return 5;
    }

    brpc::ServerOptions options;

    std::string workers;
    confParse.getValue(FLAGS_threads, workers);
    if(workers.empty()) {
        LOG(INFO) << "thread workers is default numbers";
    }
    else {
        options.num_threads = std::stoi(workers);
        LOG(INFO) << "thread workers is: " << options.num_threads;
    }

    std::string server_max_concurrency_str;
    int server_max_concurrency;
    confParse.getValue(FLAGS_server_max_concurrency, server_max_concurrency_str);
    if(server_max_concurrency_str.empty()) {
        LOG(INFO) << "server-level concurrency is unlimmit: 0";
        server_max_concurrency = 0;
    }
    else {
        server_max_concurrency = std::stoi(server_max_concurrency_str);
        LOG(INFO) << "server-level concurrency: " << server_max_concurrency;
    }
    options.max_concurrency = server_max_concurrency;

    options.method_max_concurrency = "auto";

    LOG(INFO) << "ServerOption: {num_threads:" << options.num_threads << ", "
                            <<  "max_concurrency:" << options.max_concurrency << ", "
                            <<  "method_max_concurrency:" << options.method_max_concurrency;

    std::string ip, client_listen_port, gate_listen_port;
    confParse.getValue(FLAGS_ip, ip);
    confParse.getValue(FLAGS_client_port, client_listen_port);
    confParse.getValue(FLAGS_im_gate_port, gate_listen_port);
    if(ip.empty() || client_listen_port.empty() || gate_listen_port.empty()) {
        LOG(ERROR) << "You need to configure ip and port";
        return 6;
    }

    std::string http_ip_and_port_str = ip + ":" + client_listen_port;
    std::string gate_ip_and_port_str = ip + ":" + gate_listen_port;

    if(http_server.Start(http_ip_and_port_str.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to start http_server";
        return 7;
    }
    if(server.Start(gate_ip_and_port_str.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to start server";
        return 8;
    }

    http_server.RunUntilAskedToQuit();
    server.RunUntilAskedToQuit();

    LOG(INFO) << "Server ending";
    
    delete newSink;
    
    ::YTalk::Session::freeSession();
    return 0;
}