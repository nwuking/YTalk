#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "brpc/channel.h"
#include "Session.h"
#include "protobuf/login.pb.h"

#include "gflags/gflags.h"

int main(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    logging::LogSink *newSink = YTalk::Logger::getInstance();
    logging::LogSink *oldSink = logging::SetLogSink(newSink);

    std::string url = "http://127.0.0.1:10400/HttpService/Login";

    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = "http";
    options.timeout_ms = 2000;
    options.max_retry = 3;

    if(channel.Init(url.c_str(), nullptr, &options) != 0) {
        LOG(ERROR) << "Fail to init channel";
        return 2;
    }

    brpc::Controller cntl;
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);

    cntl.http_request().uri() = url.c_str();
    std::string msg = "{\"username\" : \"nwuking\", \"password\" : \"123456789\"}";
    cntl.request_attachment().append(msg);
    channel.CallMethod(nullptr, &cntl, nullptr, nullptr, nullptr);
    if(cntl.Failed()) {
        LOG(ERROR) << cntl.ErrorText();
        return 3;
    }

    LOG(INFO) << cntl.response_attachment();
    if(newSink) {
        delete newSink;
    }
    return 0;
}