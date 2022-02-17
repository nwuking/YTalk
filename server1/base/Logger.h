/**
 * @file Logger.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef YTALK_LOGGER_H
#define YTALK_LOGGER_H

#include "LogService.h"

namespace YTalk
{

namespace base
{

#define LOG_SERVICE_INIT_0()                                LogService::init()
#define LOG_SERVICE_INIT_1(logFileName)                     LogService::init(logFileName)
#define LOG_SERVICE_INIT_2(logFileName, toFile)             LogService::init(logFileName, toFile)
#define LOG_SERVICE_INIT_3(logFileName, toFile, rollSize)   LogService::init(logFileName, toFile, rollSize)

#define GET_MACRO(_0, _1, _2, _3, NAME, ...) NAME
#define LOG_SERVICE_INIT(...) GET_MACRO(_0, ##__VA_ARGS__, LOG_SERVICE_INIT_3, LOG_SERVICE_INIT_2, LOG_SERVICE_INIT_1, LOG_SERVICE_INIT_0)(__VA_ARGS__)
#define LOG_SERVICE_STOP()                                  LogService::stop()

#define LOG_TRACE(...)      LogService::output(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)      LogService::output(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)       LogService::output(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WRANING(...)    LogService::output(LOG_LEVEL_WARNING, __FILE__, __LINE__,__VA_ARGS__)
#define LOG_ERROR(...)      LogService::output(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_SYSERROR(...)   LogService::output(LOG_LEVEL_SYSERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)      LogService::output(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)           

}   // namespace base

}   // namespace YTalk

#endif  //  YTALK_LOGGER_H