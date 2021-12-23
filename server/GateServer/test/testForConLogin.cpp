#include "ConLogin.h"
#include "base/ConfigParse.h"
#include "base/Logging.h"

int main(int argc, char *argv[])
{
    logging::LogSink *newSink = YTalk::Logger::getInstance();
    logging::LogSink *oldSink = logging::SetLogSink(newSink);

    YTalk::ConfigParse confParse;
    confParse.parse("./test.conf");
    if(!confParse.status()) {
        LOG(ERROR) << "Fail to parse configuration";
        return 1;
    }

    YTalk::ConLogin conLogin;
    if(conLogin.init(&confParse)) {
        LOG(ERROR) << "Fail to init ConLogin";
    }
    else {
        LOG(INFO) << "Success to init ConLogin";
    }
    if(newSink) {
        delete newSink;
    }
    return 0;
}