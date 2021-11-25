/*================================================================================   
 *    Date: 2021-11-20
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "./LogFile.h"

#include <unordered_map>
#include <assert.h>
#include <iostream>

#include <unistd.h>
#include <sys/stat.h>

namespace YTalk
{
/*
static const std::vector<std::string> logFileNames = {
    "/logcat.log",
    "/logcat.log.1",
    "/logcat.log.2"
};
std::unordered_map<int, std::string> logFileNames = {
    {0, "/logcat.log"},
    {1, "/logcat.log.1"},
    {2, "/logcat.log.2"},
    {3, "/logcat.log.3"}
};
*/

static char logBuffer[1024];
static std::string baseFile = "/logcat.log";
static int logFileCount = 4;

LogFile::LogFile(const int &flush, const off_t &roll, const std::string &path) 
    : _flushInterval(flush),
      _rollSize(roll),
      _basePath(path),
      _hadWrittenBytes(0),
      _count(0),
      _checkEveryN(8),
      _logFileCount(0),
      _lastFlush(0)      
{
    if(::access(_basePath.data(), W_OK) != 0) {
        ::mkdir(_basePath.data(), W_OK | R_OK | X_OK);
    }
    //std::string fileName = _basePath + logFileNames[0];
    std::string fileName = _basePath + baseFile;
    _fp = ::fopen(fileName.data(), "ae");
    std::cout << "fopen\n";
    assert(_fp);
    ::setbuffer(_fp, logBuffer, sizeof logBuffer);
}

LogFile::~LogFile() {
    flush();
    ::fclose(_fp);
}

void LogFile::append(const char *msg, int len) {
    int written = 0;
    while(written != len) {
        int remaining = len - written;
        int n = ::fwrite_unlocked(msg+written, 1, remaining, _fp);
        if(n != remaining) {
            int err = ::ferror(_fp);
            if(err) {
                ::fprintf(stderr, "LogFile::append msg fail \n");
                break;
            }
        }

        written += n;
    }
    _hadWrittenBytes += written;
    if(_hadWrittenBytes >= _rollSize) {
        flush();
        rollLogFile();
    }
    else {
        ++_count;
        if(_count > _checkEveryN) {
            _count = 0;
            time_t now = ::time(NULL);
            if(now - _lastFlush > _flushInterval) {
                _lastFlush = now;
                flush();
            }
        }
    }
}

void LogFile::flush() {
    ::fflush(_fp);
}

void LogFile::rollLogFile() {
    ++_logFileCount;
    ::fclose(_fp);
    changeLogFileName();
    //std::string names = _basePath + logFileNames[0];
    std::string names = _basePath + baseFile;
    _fp = ::fopen(names.data(), "ae");
    assert(_fp);
    ::setbuffer(_fp, logBuffer, sizeof logBuffer);
}

void LogFile::changeLogFileName() {
    if(_logFileCount > logFileCount - 1) {
        --_logFileCount;
        //std::string file = _basePath + logFileNames[_logFileCount];
        std::string file = _basePath + baseFile + std::to_string(_logFileCount);
        ::remove(file.data());
    }
    std::string newFileName, oldFileName;
    
    for(int i = _logFileCount; i > 1; --i) {
        //newFileName = _basePath + logFileNames[i];
        //oldFileName = _basePath + logFileNames[i-1];
        newFileName = _basePath + baseFile + "." + std::to_string(i);
        oldFileName = _basePath + baseFile + "." + std::to_string(i-1);
        ::rename(oldFileName.data(), newFileName.data());
    }
    newFileName = _basePath + baseFile + ".1";
    oldFileName = _basePath + baseFile;
    ::rename(oldFileName.data(), newFileName.data());
}

}    // namespace YTalk