/*================================================================================   
 *    Date: 
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <brpc/server.h>
#include <gflags/gflags.h>

#include "base/Logging.h"
#include "base/ConfigParse.h"

DEFINE_string(ip, "login_server_ip", "login server ip key");
DEFINE_string(client_port, "client_port", "for listen client");
DEFINE_string(im_gate_port, "im_gate_port", "for listen IMGateServre");
DEFINE_string(config, "./login_server.conf", "cnfig file");
DEFINE_string(log_config, "./log.conf", "log configure");

int main(int argc, char *argv[])
{
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

    //////
    //TODO

    newSink = ::logging::SetLogSink(oldSink);
    delete newSink;
    return 0;
}