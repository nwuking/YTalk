/*================================================================================   
 *    Date: 2021-11-20
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "./LogFile.h"

#include <vector>
#include <assert.h>

namespace YTalk
{

static std::vector<std::string> logFileNames = {
    "logcat.log",
    "logcat.log.1",
    "logcat.log.2",
    "logcat.log.3",
    "logcat.log.4"
};

static char logBuffer[64*1024];

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
    std::string fileName = _basePath + logFileNames[0];
    _fp = ::fopen(fileName.data(), "ae");
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
    if(_logFileCount < logFileNames.size() - 1) {
        ++_logFileCount;
    }
    ::fclose(_fp);
    changeLogFileName(_logFileCount);
    std::string names = _basePath + logFileNames[0];
    _fp = ::fopen(names.data(), "ae");
    assert(_fp);
    ::setbuffer(_fp, logBuffer, sizeof logBuffer);
}

void LogFile::changeLogFileName(int n) {
    if(n == logFileNames.size() - 1) {
        std::string file = _basePath + logFileNames[n];
        ::remove(file.data());
    }
    for(int i = n; i > 0; --i) {
        std::string newFileName = _basePath + logFileNames[n];
        std::string oldFileName = _basePath + logFileNames[n-1];
        ::rename(oldFileName.data(), newFileName.data());
    }
}

}    // namespace YTalk