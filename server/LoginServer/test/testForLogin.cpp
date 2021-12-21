#include "LoginService.h"
#include "base/Logging.h"
#include "base/ConfigParse.h"
#include "Session.h"

#include "brpc/channel.h"
#include "brpc/server.h"
#include "gflags/gflags.h"

DEFINE_string(test_conf, "./test.conf", "");

int main(int argc, char *argv[]) 
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance();
    ::logging::LogSink *oldSink = ::logging::SetLogSink(oldSink);

    ::YTalk::ConfigParse cParse;
    cParse.parse(FLAGS_test_conf);

    ::YTalk::Session *session = ::YTalk::Session::getInstance();
    struct in_addr ip = {};
    session->record("nwu", "520", ip);

    YTalk::LoginServiceImpl pp;
    pp.init(&cParse, session);

    ::brpc::Controller cntl;
    std::string msg = "{\"username\" :\"nwuking\", \"password\" : \"123456789\"}";
    cntl.request_attachment().append(msg);

    pp.Login(&cntl, nullptr, nullptr, nullptr);

    LOG(INFO) << "STATUS: " << cntl.http_response().status_code();
    LOG(INFO) << "MSG   : " << cntl.response_attachment();

    delete newSink;
    ::YTalk::Session::freeSession();
    return 0;
}