/*================================================================================   
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include <butil/logging.h>    // brpc

namespace YTalk
{

class Logger : public ::logging::LogSink
{
public:
    Logger();
    ~Logger();

    bool OnLogMessage(int serverity, const char *file, int line, const butil::StringPiece &content) override;

    void setLogFilePath(const char *path);

    void rollLogFiles();

private:
    const char *_logFilePath;
};    // class Logger

}    // namespace YTalk