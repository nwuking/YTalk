/*================================================================================   
 *    Date: 2021-12-27
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <gflags/gflags.h>
#include <brpc/server.h>

#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "ConLogin.h"
#include "ConRoute.h"
#include "Channels.h"
#include "GateService.h"

DEFINE_string(ip, "gate_server_listen_ip", "");
DEFINE_string(port, "gate_server_listen_port", "");
DEFINE_string(log_conf, "./gate_server_log.conf", "");
DEFINE_string(server_conf, "./gate_server.conf", "");
DEFINE_string(threads, "num_threads", "");
DEFINE_string(server_max_concurrency, "server_max_concurrency", "sever-level");

int main(int argc, char *argv[]) 
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    signal(SIGPIPE, SIG_IGN);

    ::logging::LogSink *newSink = YTalk::Logger::getInstance(FLAGS_log_conf.c_str());
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);

    YTalk::ConfigParse confParse;
    confParse.parse(FLAGS_server_conf);
    if(!confParse.status()) {
        LOG(ERROR) << "Fail to parse config";
        return 1;
    }

    YTalk::ConLogin conLogin;
    if(conLogin.init(&confParse)) {
        LOG(ERROR) << "Fail to init ConLogin";
        return 2;
    }

    YTalk::ConRoute conRoute;
    if(conRoute.init(&confParse)) {
        LOG(ERROR) << "Fail to init ConRoute";
        return 3;
    }

    YTalk::Channels channels;
    if(channels.init(&confParse)) {
        LOG(ERROR) << "Fail to init Channels";
        return 4;
    }

    YTalk::GateServiceImpl gateServiceImpl;
    if(gateServiceImpl.init(&confParse, &conLogin, &channels, &conRoute)) {
        LOG(ERROR) << "Fail to init GateServiceImpl";
        return 5;
    }

    brpc::Server server;

    if(server.AddService(&gateServiceImpl, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add GateServiceImpl";
        return 6;
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

    std::string ip, port;
    confParse.getValue(FLAGS_ip, ip);
    confParse.getValue(FLAGS_port, port);
    if(ip.empty() || port.empty()) {
        LOG(ERROR) << "You need to configure ip and port";
        return 6;
    }

    std::string ip_and_port_str = ip + ":" + port;
    if(server.Start(ip_and_port_str.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to start server";
        return 7;
    }
    
    server.RunUntilAskedToQuit();
    
    if(newSink) {
        delete newSink;
    }
    return 0;
}