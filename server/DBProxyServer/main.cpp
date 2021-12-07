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

#include <iostream>

DEFINE_string(config, "./config.conf", "the config of the DBProxyServer");
DEFINE_string(log_config_path, "log_config_path", "the key of log_config_path");
DEFINE_string(ip, "ip", "ip address of server");
DEFINE_string(port, "port", "port of server");

int main(int argc, char *argv[])
{
    //google::ParseCommandLineFlags(&argc, &argv, true);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    signal(SIGPIPE, SIG_IGN);

    ::YTalk::ConfigParse confParse;
    confParse.parse(FLAGS_config);
    if(!confParse.status()) {
        std::cout << "Faild to parse config, please check it!" << std::endl;
        return -1;
    }

    std::string logConfig;
    confParse.getValue(FLAGS_log_config_path, logConfig);
    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance(logConfig.data());
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);
    if(oldSink) {
        delete oldSink;
    }

    ::YTalk::MySqlServiceImpl mySqlService;
    //TODO
    
    if(newSink) {
        delete newSink;
    }
    return 0;
}