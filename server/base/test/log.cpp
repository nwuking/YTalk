#include "base/Logging.h"

#include <iostream>
#include <string>

int main() {

    std::string configPath = "./log.conf";

    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance(configPath.data());
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);
    if(oldSink) {
        delete oldSink;
    }

    for(int i = 0; i < 11; ++i) {
        LOG(INFO) << "i:" << i;
    }

    //::sleep(5);
    return 0;
}