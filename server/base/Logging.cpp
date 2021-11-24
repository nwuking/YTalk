/*================================================================================   
 *    Date: 2021-11-14
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "./Logging.h"
#include "./Time.h"
#include "./AsyncLog.h"
#include "./ConfigParse.h"

#include <bthread/bthread.h>    // brpc
//#include <butil/memory/singleton.h>    // brpc

#include <ostream>
#include <vector>
#include <memory>
#include <string>

const std::vector<std::string> LogLevelStr = {"INFO",
                                              "NOTICE",
                                              "WARNING",
                                              "ERROR",
                                              "FATAL"};

#define LOG_FLUSH_INTERVAL "log_flush_interval"
#define LOG_ROLL_SIZE      "log_roll_size"
#define LOG_FILE_PATH      "log_file_path"

namespace YTalk
{

static const int kFLUSHINTERVAL = 3;    // s
static const int kROLLSIZE = 8000;      // bytes
static const char* kLOGFILEPATH = "../data/logs/";    //

//static pthread_once_t g_pthread_once = PTHREAD_ONCE_INIT;

static std::unique_ptr<AsyncLog> g_AsyncLog;

static void asyncLogInit(const int &flush, const off_t &roll, const std::string &path) {
    g_AsyncLog.reset(new AsyncLog(flush, roll, path));
    g_AsyncLog->start();
}

void g_output(const std::string &log) {
    //pthread_once(&g_pthread_once, asyncLogInit);
    g_AsyncLog->append(log.c_str(), log.size());
}

Logger* Logger::getInstance(const char *config) {
    //return Singleton<Logger, LeakySingletonTraits<Logger> >::get();
    return new Logger(config);
}

Logger::Logger(const char *config) {
    // read from config
    std::cout << "start init Logger\n";
    int flush = kFLUSHINTERVAL;
    off_t rollSize = kROLLSIZE;
    std::string path = kLOGFILEPATH;
    if(config != nullptr) {
        // read from config
        std::cout << "Logger --> parse log.conf\n";
        ConfigParse confP;
        confP.parse(config);
        if(confP.isExist(LOG_FLUSH_INTERVAL)) {
            confP.getValue(LOG_FLUSH_INTERVAL, flush);
        }
        if(confP.isExist(LOG_ROLL_SIZE)) {
            confP.getValue(LOG_ROLL_SIZE, rollSize);
        }
        if(confP.isExist(LOG_FILE_PATH)) {
            confP.getValue(LOG_FILE_PATH, path);
        }
    }
    std::cout << "start init asyncLog\n";
    asyncLogInit(flush, rollSize, path);
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