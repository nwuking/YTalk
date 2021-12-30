/*================================================================================   
 *    Date: 2021-12-26
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "gflags/gflags.h"
#include "brpc/server.h"

#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "IMService.h"
#include "ConRoute_im.h"

DEFINE_string(server_config, "im_server.conf", "");
DEFINE_string(log_config, "im_server_log.conf", "");
DEFINE_string(threads, "num_threads", "");
DEFINE_string(server_max_concurrency, "server_max_concurrency", "sever-level");
DEFINE_string(ip, "im_server_listen_ip", "");
DEFINE_string(port, "im_server_listen_port", "");

int main(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    signal(SIGPIPE, SIG_IGN);

    logging::LogSink *newSink = YTalk::Logger::getInstance(FLAGS_log_config.c_str());
    logging::LogSink *oldSink = logging::SetLogSink(newSink);

    YTalk::ConfigParse confParse;
    confParse.parse(FLAGS_server_config);
    if(!confParse.status()) {
        LOG(ERROR) << "Fail to parse route_server.conf";
        return 1;
    }

    YTalk::ConRoute conRoute;
    if(conRoute.init(&confParse)) {
        LOG(ERROR) << "Fail to init ConRoute";
        return 2;
    }

    YTalk::IMServiceImpl imServiceImpl;
    if(imServiceImpl.init(&confParse, &conRoute)) {
        LOG(ERROR) << "Fail to init IMServiceImpl";
        return 3;
    }

    brpc::Server server;

    if(server.AddService(&imServiceImpl, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add IMService to server";
        return 4;
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

    //TODO
    if(newSink) {
        delete newSink;
    }
    return 0;
}