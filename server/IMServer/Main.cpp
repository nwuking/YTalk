/*================================================================================   
 *    Date: 2021-12-26
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "gflags/gflags.h"
#include "brpc/server.h"

#include "base/Logging.h"
#include "base/ConfigParse.h"

DEFINE_string(server_config, "im_server.conf", "");
DEFINE_string(log_config, "im_server_log.conf", "");

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

    //TODO
    if(newSink) {
        delete newSink;
    }
    return 0;
}