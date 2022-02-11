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

#ifndef YTalk_LOG_SERVICE_H
#define YTalk_LOG_SERVICE_H

#include <cstdint>
#include <cstdio>
#include <string>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <list>

namespace YTalk
{

namespace base
{

#define LOG_API

enum LOG_LEVEL
{
    LOG_LEVEL_TRACE, 
    LOG_LEVEL_DEBUG, 
    LOG_LEVEL_INFO, 
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR, 
    LOG_LEVEL_SYSERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_NUM
};  // enum LOG_LEVEL

class LOG_API LogService 
{
public:
    /**
     * @brief 初始化LogService，启动一个线程负责写log
     * 
     * @param logFileName 日志文件的名称
     * @param rollSize 单个日志文件大小的期望最大值
     * @param toFile 日志写到文件还是控制台
     * @return true 初始化成功
     * @return false 初始化失败
     */
    static bool init(const char *logFileName = nullptr, bool toFile = true, std::int64_t rollSize = 1024*1024*22);
    /**
     * @brief 停止LogService
     * 
     */
    static void stop();
    /**
     * @brief 设置Log_Level
     * 
     * @param logLevel 
     */
    static void setLogLevel(LOG_LEVEL logLevel);
    /**
     * @brief 
     * 
     * @param logLevel 日志级别
     * @param fileName 所在的文件名
     * @param lineNo 所在的行号
     * @param fmt 输出格式
     * @param ... 
     */
    static void output(LOG_LEVEL logLevel, const char *fileName, std::int32_t lineNo, const char *fmt, ...);
private:
    LogService() = delete;
    ~LogService() = delete;
    LogService(const LogService &obj) = delete;
    LogService& operator=(const LogService &obj) = delete;

    /**
     * @brief LogService后台线程函数，单独起一个线程，负责写日志
     * 
     */
    static void threadProc();
    /**
     * @brief make一条log的前面部分：[时间]-[线程号]-[log_level]: {
     * 
     * @param logLevel 线程级别
     * @param prefixLog 返回值
     */
    static void makePrefixLog(LOG_LEVEL logLevel, std::string &prefixLog);
    /**
     * @brief make一条log的后面部分：} fileName: lineNo. 
     * 
     * @param fileName 所在文件名
     * @param lineNo 所在行号
     * @param suffixLog 返回值
     */
    static void makeSuffixLog(const char *fileName, std::int32_t lineNo, std::string &suffixLog);
    /**
     * @brief 负责写日志
     * 
     * @param log 一条完整的日志
     * @return true 写成功
     * @return false 写失败
     */
    static bool write(const std::string &log);
    /**
     * @brief 主动让程序退出
     * 
     */
    static void crash();
    /**
     * @brief 创建一个文件
     * 
     * @param newFileName 要创建的文件名
     */
    static void createLogFile(const std::string &newFileName);

private:
    static std::int64_t                     m_rollSize;      // 单个日志文件的最大字节数
    static bool                             m_toFile;        // 日志写到file还是console
    static std::string                      m_logFileName;   // 日志文件名
    static std::unique_ptr<std::thread>     m_threadPtr;     // 负责写日志
    static bool                             m_isExited;      
    static std::mutex                       m_mutext;
    static std::condition_variable          m_cond;
    static FILE*                            m_fpPtr;
    static LOG_LEVEL                        m_curLogLevel;
    static std::list<std::string>           m_logBuffer;    // 保存待写入的日志
    static bool                             m_isRunning;    // 
    static std::int64_t                     m_hadWriteen;   // 以写的字节数

    static std::string                      m_logLevelStr[LOG_LEVEL_NUM];

};  // class LogService

}  // namespace base

}  //namespace YTalk

#endif