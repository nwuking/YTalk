#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "brpc/channel.h"
#include "base/structs.h"
#include "protobuf/redis.pb.h"

int main()
{
    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance();
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);

    const char *server_ip_and_port = "127.0.0.1:10600";
    brpc::ChannelOptions options;
    options.max_retry = 3;
    options.connection_type = "single";
    options.timeout_ms = 200;
    brpc::Channel channel;

    DBProxyServer::RedisRequest request;
    DBProxyServer::RedisResponse response;
    brpc::Controller cntl;

    if(channel.Init(server_ip_and_port, nullptr) != 0) {
        LOG(ERROR) << "Fail to init Channel";
        delete newSink;
        return -1;
    }

    DBProxyServer::RedisService_Stub stub(&channel);

    std::string msg = "{"
                            "\"u_name\": \"18176447770\", "
                            "\"token\": \"mytoken\""
                      "}";
    request.set_message(msg);

    stub.SetToken(&cntl, &request, &response, nullptr);

    if(!cntl.Failed()) {
        if(response.status() == YTalk::REDIS_SUCCESS) {
            LOG(INFO) << "token: " << response.message();
        }
    }
    else {
        LOG(ERROR) << "call fail";
    }

    delete newSink;
    return 0;
}