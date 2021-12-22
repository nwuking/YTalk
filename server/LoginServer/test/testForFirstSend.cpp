#include "LoginService.h"
#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "Session.h"
#include "AuthService.h"

#include "brpc/channel.h"
#include "brpc/server.h"
#include "gflags/gflags.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

DEFINE_string(test_conf, "./test.conf", "");

int main(int argc, char *argv[]) 
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance();
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);

    ::YTalk::ConfigParse cParse;
    cParse.parse(FLAGS_test_conf);

    ::YTalk::Session *session = ::YTalk::Session::getInstance();

    YTalk::AuthServiceImpl pp;
    pp.init(&cParse, session);

    ::brpc::Controller cntl;
    std::string msg = "{\"name\" :\"nwuking\", \"port\" : 5200}";

    rapidjson::Document document;

    document.Parse(msg.c_str());
        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
        document.Accept(writer);

        msg = sb.GetString();

    ::LoginServer::AuthRequest request;
    request.set_message(msg.c_str());

    pp.FirstSend(&cntl, &request, nullptr, nullptr);

    //pp.Login(&cntl, nullptr, nullptr, nullptr);

    ::YTalk::GateServerMsg *gg = session->getGateServerMsg();
    if(gg) {
        LOG(INFO) << "IP:" << gg->_ip << "   port: " << gg->_port;
    }

    delete newSink;
    ::YTalk::Session::freeSession();
    return 0;
}