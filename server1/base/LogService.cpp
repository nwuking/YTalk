/**
 * @file LogService.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "LogService.h"
#include "ThreadUtils.h"

#include <cstdarg>
#include <sys/timeb.h>
#include <time.h>

namespace YTalk
{

namespace base
{

std::int64_t LogService::m_rollSize = 1024*1024*22;
bool LogService::m_toFile = true;
std::string LogService::m_logFileName = "log";
std::unique_ptr<std::thread> LogService::m_threadPtr;
bool LogService::m_isExited = false;
std::mutex LogService::m_mutext;
std::condition_variable LogService::m_cond;
FILE* LogService::m_fpPtr = nullptr;
LOG_LEVEL LogService::m_curLogLevel = LOG_LEVEL_INFO;
std::list<std::string> LogService::m_logBuffer;
bool LogService::m_isRunning = false;
std::int64_t LogService::m_hadWriteen = 0;
std::string LogService::m_logLevelStr[LOG_LEVEL_NUM] = {    "[TRACE   ]", 
                                                            "[DEBUG   ]", 
                                                            "[INFO    ]", 
                                                            "[WARNING ]", 
                                                            "[ERROR   ]", 
                                                            "[SYSERROR]", 
                                                            "[FATAL   ]"};

bool LogService::init(const char *logFileName/*= nullptr*/, bool toFile/*= true*/, std::int64_t rollSize/*= 1024*1024*22*/) {
    if(m_isRunning) {
        return true;
    }
    
    m_rollSize = rollSize;
    m_toFile = toFile;

    if(logFileName != nullptr && logFileName[0] != 0) {
        m_logFileName = logFileName;
    }
    else if(m_logFileName.empty()) {
        m_toFile = false;
    }

    m_threadPtr.reset(new std::thread(threadProc));

    return true;
}

void LogService::stop() {
    m_isExited = true;
    m_cond.notify_one();

    if(m_threadPtr->joinable()) {
        m_threadPtr->join();
    }

    if(!m_fpPtr) {
        fclose(m_fpPtr);
        m_fpPtr = nullptr;
    }
}

void LogService::setLogLevel(LOG_LEVEL logLevel) {
    if(logLevel < LOG_LEVEL_TRACE || logLevel > LOG_LEVEL_FATAL) {
        return;
    }
    
    m_curLogLevel = logLevel;
}

void LogService::output(LOG_LEVEL logLevel, const char *fileName, std::int32_t lineNo, const char *fmt, ...) {
    if(logLevel < m_curLogLevel) {
        return;
    }

    std::string prefixLogMsg;    // 日志正文的前缀部分
    makePrefixLog(logLevel, prefixLogMsg);

    va_list args;
    va_start(args, fmt);
    int logMsgLen = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);

    char logMsg[logMsgLen+1];   // 存放日志正文
    va_list args1;
    va_start(args1, fmt);
    vsnprintf(logMsg, logMsgLen+1, fmt, args1);
    va_end(args1);

    std::string suffixLogMsg;  // 日志后缀
    makeSuffixLog(fileName, lineNo, suffixLogMsg);

    // 一条完整的日志
    std::string log = prefixLogMsg + logMsg + suffixLogMsg + "\n";

    if(logLevel != LOG_LEVEL_FATAL) {
        std::lock_guard<std::mutex> lock_guard(m_mutext);
        m_logBuffer.push_back(log);
        m_cond.notify_one();
    }
    else {
        // 输出日志后，让整个程序退出
        std::lock_guard<std::mutex> lock_guard(m_mutext);
        while(!m_logBuffer.empty()) {
            std::string l_log = m_logBuffer.front();
            m_logBuffer.pop_front();
            write(l_log);
        }

        m_toFile = false;
        write(log);
        crash();      // 主动让程序退出
    }
}

void LogService::threadProc() {
    m_isRunning = true;

    while(true) {
        if(m_toFile) {
            //写到文件
            if(m_fpPtr == nullptr || m_hadWriteen >= m_rollSize) {
                // 新建一个文件
                m_hadWriteen = 0;

                std::string newLogFileName(m_logFileName);
                time_t now = time(nullptr);
                tm t;
                localtime_r(&now, &t);
                char timeStr[64] = {0};
                strftime(timeStr, 64, "%Y%m%d%H%M%S", &t);
                newLogFileName += timeStr;
                newLogFileName += ".log";

                createLogFile(newLogFileName);
            }
        }
        
        std::string log;
        {
            std::unique_lock<std::mutex> lock_guard(m_mutext);
            while(m_logBuffer.empty()) {
                if(m_isExited) {
                    return;
                }
                m_cond.wait(lock_guard);
            }
            log = m_logBuffer.front();
            m_logBuffer.pop_front();
        }

        if(!write(log)) {
            break;
        }

        m_hadWriteen += log.size();
    }

    m_isRunning = false;
}      // func threadProc

void LogService::makePrefixLog(LOG_LEVEL logLevel, std::string &prefixLog) {
    std::string strLevel = m_logLevelStr[logLevel];

    // 时间
    char timeStr[64] = {0};
    struct timeb tp;
    ftime(&tp);
    time_t now = tp.time;
    tm time;
    localtime_r(&now, &time);
    snprintf(timeStr, 60, "[%04d-%02d-%02d %02d:%02d:%02d:%03d]", time.tm_year + 1900, 
                                                                    time.tm_mon + 1, 
                                                                    time.tm_mday, 
                                                                    time.tm_hour, 
                                                                    time.tm_min, 
                                                                    time.tm_sec, 
                                                                    tp.millitm);
    
    //线程
    std::int32_t tid = static_cast<std::int32_t>(getCurrentThreadId());
    char tidStr[32];
    snprintf(tidStr, 32, "[%08d]", tid);

    prefixLog = timeStr;
    prefixLog += "-";
    prefixLog += tidStr;
    prefixLog += "-";
    prefixLog += strLevel;
    prefixLog += ": {";
}   // func makePrefixLog

void LogService::makeSuffixLog(const char *fileName, std::int32_t lineNo, std::string &suffixLog) {
    suffixLog = "} ";
    suffixLog += fileName;
    suffixLog += ": ";
    suffixLog += std::to_string(lineNo);
    suffixLog += ".";
}    // func makeSuffixLog

bool LogService::write(const std::string &log) {
    if(m_fpPtr == nullptr && m_toFile) {
        // 以防万一
        std::string newLogFileName(m_logFileName);
        time_t now = time(nullptr);
                tm t;
                localtime_r(&now, &t);
                char timeStr[64] = {0};
                strftime(timeStr, 64, "%Y%m%d%H%M%S", &t);
                newLogFileName += timeStr;
        newLogFileName += ".log";
        createLogFile(newLogFileName);
    }
    if(m_toFile) {
        //写到文件
        const char *curPtr = log.c_str();
        std::int64_t ret = 0;
        while(true) {
            ret = fwrite(curPtr, 1, log.size()-ret, m_fpPtr);
            if(ret <= 0) {
                return false;
            }
            if(ret == log.size()) {
                return true;
            }
            curPtr += ret;
        }
    }
    else {
        //写到console
        printf(log.c_str());
    }
    return true;
}

void LogService::crash() {
    printf("crsah\n");
    char *p = nullptr;
    *p = 0;
}

void LogService::createLogFile(const std::string &newLogFile) {
    if(m_fpPtr != nullptr) {
        fclose(m_fpPtr);
    }

    m_fpPtr = fopen(newLogFile.c_str(), "w+");
}

}  // namespace base

}  // namespace YTalk