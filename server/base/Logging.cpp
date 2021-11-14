/*================================================================================   
 *    Date: 2021-11-14
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "./Logging.h"
#include "./Time.h"
#include "./AsyncLog.h"

#include <bthread/bthread.h>    // brpc
#include <butil/memory/singleton.h>    // brpc

#include <ostream>
#include <vector>
#include <memory>

const std::vector<std::string> LogLevelStr = {"INFO",
                                              "NOTICE",
                                              "WARNING",
                                              "ERROR",
                                              "FATAL"};

namespace YTalk
{

static pthread_once_t g_pthread_once = PTHREAD_ONCE_INIT;

static std::unique_ptr<AsyncLog> g_AsyncLog;

static void asyncLogInit() {
    g_AsyncLog.reset(new AsyncLog);
    g_AsyncLog->start();
}

void g_output(const std::string &log) {
    pthread_once(&g_pthread_once, asyncLogInit);
    g_AsyncLog->append(log.c_str(), log.size());
}

Logger* Logger::getInstance() {
    return Singleton<Logger, LeakySingletonTraits<Logger> >::get();
}

bool Logger::OnLogMessage(int serverity, const char *file, int line, const butil::StringPiece &content) {
    //线程号 日期时间 [级别] content file line
    std::ostringstream os;
    bthread_t tid = bthread_self();
    Time time = Time::now();
    std::string timeStr = time.timeToStringLocal();
    os << timeStr << " " << tid << " [" << LogLevelStr[serverity] << "] " 
       << content << " " << file << ":" << line << "\n";

    if(LogLevelStr[serverity] != "FATAL") {
        butil::AutoLock lock_guard(_lock);
        g_output(os.str());
    }
    else {
        //FATAL,致命错误打在stderr
        fwrite(os.str().data(), os.str().size(), 1, stderr);
        fflush(stderr);
    }
}

}    // namespace YTalk