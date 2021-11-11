/*================================================================================   
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "./Logging.h"

#include <bthread/bthread.h>    // brpc

#include <ostream>

namespace YTalk
{

Logger::Logger() : _logFilePath(nullptr) {
    //
}

Logger::~Logger() {
    //
}

bool Logger::OnLogMessage(int serverity, const char *file, int line, const butil::StringPiece &content) {
    //线程号 日期时间 [级别] content file line
    std::ostringstream os;
    //
    // TODO
}

}    // namespace YTalk