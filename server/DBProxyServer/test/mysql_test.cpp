#include "MySqlService.h"
//#include "base/ConfigParse.h"
#include "base/Logging.h"

#include <iostream>
#include <string>

int main() 
{
    ::logging::LogSink *newSink = ::YTalk::Logger::getInstance();
    ::logging::LogSink *oldSink = ::logging::SetLogSink(newSink);

    ::YTalk::MySqlServiceImpl mysqlService;
    if(mysqlService.init("./test.conf")) {
        std::cout << "MySqlServiceImpl init failed\n";
        return -1;
    }

    std::string insert1 = "insert into beauty values(1, 'zyh', 2)";
    std::string insert2 = "insert into beauty values(2, 'wyhy', 1)";
    std::string query = "select * from beauty";
    //mysqlService. ("YTalk_test", insert1.c_str());
    //mysqlService.query("YTalk_test", query.c_str());

    newSink = ::logging::SetLogSink(oldSink);
    delete newSink;
    return 0;
}