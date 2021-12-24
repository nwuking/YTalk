/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <gflags/gflags.h>
#include <brpc/server.h>

#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "ConLogin.h"
#include "ConRoute.h"

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
    //TODO
    if(newSink) {
        delete newSink;
    }
    return 0;
}