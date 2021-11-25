#include "base/Logging.h"

#include <iostream>
#include <string>

int main() {

    std::string configPath = "./log.conf";

    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance(configPath.data());
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);

    for(int i = 0; i < 2222; ++i) {
        LOG(INFO) << "i:" << i;
    }

    ::sleep(5);
 std::cout << "end\n";
    newSink = ::logging::SetLogSink(oldSink);
    if(newSink) {
        delete newSink;
    }
   //std::cout << "end\n";
    return 0;
}