#include "RedisService.h"
#include "base/Logging.h"
#include "base/ConfigParse.h"

int main()
{
    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance();
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);

    ::YTalk::RedisServiceImpl rService;
    if(rService.init("./test.conf")) {
        LOG(ERROR) << "RedisserviceImpl init failed";
        delete newSink;
        return 1;
    }

    std::string key = "yyj";
    std::string value = "zym";
    std::string redis = "Redis_test";

    rService.set(redis, key, value);
    if(rService.get(redis, key) == value) {
        LOG(INFO) << "test successful";
    }

    newSink = ::logging::SetLogSink(oldSink);
    delete newSink;
    return 0;
}