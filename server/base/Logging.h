/*================================================================================   
 *    Date: 2021-11-14
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <butil/logging.h>    // brpc
#include <butil/synchronization/lock.h>    // brpc

namespace YTalk
{

class Logger : public ::logging::LogSink
{
public:
    static Logger* getInstance();

    bool OnLogMessage(int serverity, const char *file, int line, const butil::StringPiece &content) override;

private:
    Logger() {}
    ~Logger() {}

    butil::Lock _lock;
};    // class Logger

}    // namespace YTalk