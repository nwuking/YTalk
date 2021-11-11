//#include <mysql/mysql.h>
#include <iostream>
//#include <gflags/gflags.h>
#include <butil/logging.h>
#include <ostream>
#include <bthread/bthread.h>
#include <butil/time/time.h>

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
        std::cout << content << "\n";
        std::cout << file << "  :  " << line << "\n";
        std::cout << bthread_self() << "\n";
        butil::Time time = butil::Time::NowFromSystemTime();
        time_t t = time.ToTimeT();
        struct tm *pp = localtime(&t);
        std::cout << pp->tm_year << "/" << pp->tm_mon << "/ " << pp->tm_mday
                    << ": " << pp->tm_hour << "-" << pp->tm_min << "\n";
    }
};

int main() 
{
    //LOG_EVERY_SECOND(INFO) << "High-frequent logs";
    ::logging::LogSink *logsink = new Logger();
    ::logging::LogSink *oldsink = ::logging::SetLogSink(logsink);
    if(oldsink) {
        delete oldsink;
    }

    std::ostringstream os;

    LOG(INFO) << "nwuking" << "...";
    return 0;
}

