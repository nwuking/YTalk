/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <gflags/gflags.h>
#include <brpc/server.h>

#include "base/Logging.h"
#include "base/ConfigParse.h"

DEFINE_string(ip, "gate_server_listen_ip", "");
DEFINE_string(port, "gate_server_listen_port", "");
DEFINE_string(log_conf, "./gate_server_log.conf", "");
DEFINE_string(server_conf, "./gate_server.conf", "");

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
    //TODO
    if(newSink) {
        delete newSink;
    }
    return 0;
}