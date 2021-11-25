/*================================================================================   
 *    Date: 2021-11-20
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <string>

#include <stdio.h>

namespace YTalk
{

class LogFile
{
public:
    LogFile(const int &flush, const off_t &roll, const std::string &path);
    ~LogFile();

    void append(const char *msg, int len);

    void flush();

    void rollLogFile();

    void changeLogFileName();
    //TODO

private:
    int _flushInterval;
    off_t _rollSize;
    std::string _basePath;

    FILE *_fp;
    int _hadWrittenBytes;
    int _count;
    int _checkEveryN;
    int _logFileCount;

    time_t _lastFlush;

    //static std::unordered_map<int, std::string> logFileNames;

};    // class LogFile

}    // namespace YTalk