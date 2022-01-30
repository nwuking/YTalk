//#include "MySqlService.h"
#include "base/Logging.h"
#include "brpc/channel.h"
#include "protobuf/mysql.pb.h"
#include "base/structs.h"

int main()
{
    logging::LogSink *newSink = YTalk::Logger::getInstance();
    logging::LogSink *oldSink = logging::SetLogSink(newSink);

    const char *server_ip_and_port = "127.0.0.1:10600";
    brpc::ChannelOptions options;
    options.max_retry = 3;
    options.connection_type = "single";
    options.timeout_ms = 200;
    brpc::Channel channel;

    
    DBProxyServer::MySqlRequest request;
    DBProxyServer::MySqlResponse response;
    brpc::Controller cntl;
    std::string msg = "{"
                            "\"u_id\": 20220130, "
                            "\"u_name\": \"18176447770\", "
                            "\"u_nickname\": \"nwuking\", "
                            "\"u_password\": \"zym520\", "
                            "\"u_gender\": \"1\", "
                            "\"u_birthday\": 20000210"
                      "}";
    request.set_message(msg);

    if(channel.Init(server_ip_and_port, nullptr) != 0) {
        LOG(ERROR) << "Fail to init Channel";
        delete newSink;
        return -1;
    }
DBProxyServer::MySqlService_Stub stub(&channel);
    //stub.GetMaxUserId(&cntl, &request, &response, nullptr);
    stub.Register(&cntl, &request, &response, nullptr);
    if(!cntl.Failed()) {
        if(response.status() == YTalk::DBPROXY_SUCCESS) {
            LOG(INFO) << "TRUE";
        }
        else {
            LOG(ERROR) << "FALSE";
        }
    }
    else {
        LOG(ERROR) << "call error";
    }
    delete newSink;
    return 0;
}