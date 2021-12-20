#include "AccessMySql.h"
#include "base/Logging.h"
#include "base/ConfigParse.h"

int main()
{
    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance();
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);

    ::YTalk::AccessMySql p;
    ::YTalk::ConfigParse c;
    c.parse("./test.conf");
    p.init(&c);

    std::string user = "nwuking";
    std::string pass = "123456789";

    if(p.queryForLogin(user, pass) == 0) {
        LOG(INFO) << "OK";
    }
    else {
        LOG(INFO) << "FAIL"
    }

    newSink = ::logging::SetLogSink(oldSink);
    if(!newSink) {
        delete newSink;
    }
    return 0;
}