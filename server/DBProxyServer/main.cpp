//#include <mysql/mysql.h>
#include <iostream>
//#include <gflags/gflags.h>
#include <butil/logging.h>
#include <ostream>
#include <bthread/bthread.h>
#include <butil/time/time.h>
#include <bthread/bthread.h>

class Logger : public ::logging::LogSink
{
    public:
    Logger() = default;
    ~Logger() = default;

    bool OnLogMessage(int serverity,
                      const char *file,
                      int line,
                      const butil::StringPiece &content)
    {
        std::cout << content << ":" << this << "\n";
    }
};

void* func1(void *t) {
    bthread_t id = bthread_self();
    LOG(INFO) << "func1--" << id;
}

void* func2(void *t) {
    bthread_t id = bthread_self();
    LOG(INFO) << "func2--" << id;
}

int main() 
{
    //LOG_EVERY_SECOND(INFO) << "High-frequent logs";
    ::logging::LogSink *logsink = new Logger();
    ::logging::LogSink *oldsink = ::logging::SetLogSink(logsink);
    if(oldsink) {
        delete oldsink;
    }

    bthread_t t1, t2;
    bthread_start_background(&t1, nullptr, func1, nullptr);
    bthread_start_background(&t2, nullptr, func2, nullptr);

    bthread_join(t1, nullptr);
    bthread_join(t2, nullptr);
    return 0;
}

