#include "base/Logging.h"

#include <bthread/bthread.h>

#include <iostream>
#include <string>

void* func1(void *arg) {
    for(int i = 0; i < 20000; ++i) {
        LOG(INFO) << "func1: " << i;
    }
}

void* func2(void *arg) {
    for(int i = 0; i < 20000; ++i) {
        LOG(INFO) << "func1: " << i;
    }
}

int main() {

    std::string configPath = "./log.conf";

    //::logging::LogSink *newSink = ::YTalk::Logger::getInstance(configPath.data());
    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance();
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);

   bthread_t t1, t2;
   bthread_start_background(&t1, nullptr, func1, nullptr);
   bthread_start_background(&t2, nullptr, func2, nullptr);

    for(int i = 0; i < 2222; ++i) {
        LOG(INFO) << "i:" << i;
    }

   bthread_join(t1, nullptr);
   bthread_join(t2, nullptr);

   newSink = ::logging::SetLogSink(oldSink);
    if(newSink) {
        delete newSink;
    }
   //std::cout << "end\n";
    return 0;
}