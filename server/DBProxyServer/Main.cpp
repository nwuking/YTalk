/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <brpc/server.h>
#include <gflags/gflags.h>

#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "MySqlService.h"
#include "RedisService.h"

DEFINE_string(config, "./db_proxy_server.conf", "the config of the DBProxyServer");
DEFINE_string(log_config, "./db_proxy_log.conf", "the config of log");
DEFINE_string(ip, "server_listen_ip", "ip address of server");
DEFINE_string(port, "server_listen_port", "port of server");
DEFINE_string(threads, "num_threads", "the number of worker threads");
DEFINE_string(server_max_concurrency, "server_max_concurrency", "sever-level");

int main(int argc, char *argv[])
{
    //google::ParseCommandLineFlags(&argc, &argv, true);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    signal(SIGPIPE, SIG_IGN);

    /// for LOG
    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance(FLAGS_log_config.c_str());
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);

    ::YTalk::ConfigParse confParse;
    confParse.parse(FLAGS_config);
    if(!confParse.status()) {
        return 1;
    }

    ::YTalk::MySqlServiceImpl mysqlService;
    if(mysqlService.init(&confParse)) {
        LOG(ERROR) << "MySqlServiceImpl init failed";
        return 2;
    }
    /*
    ::YTalk::RedisServiceImpl redisService;
    if(redisService.init(&confParse)) {
        LOG(ERROR) << "RedisServiceImpl init failed";
        return 3;
    }*/

    brpc::Server server;

    if(server.AddService(&mysqlService, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service: MySqlServiceImpl";
        return 4;
    }
    /*
    if(server.AddService(&redisService, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service: RedisServiceImpl";
        return 5;
    }*/

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

    newSink = ::logging::SetLogSink(oldSink);
    if(newSink) {
        delete newSink;
    }
    return 0;
}