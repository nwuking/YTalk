/*================================================================================   
 *    Date:
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_ASYNCLOG_H
#define YTALK_ASYNCLOG_H

namespace YTalk
{

class AsyncLog
{
public:
    AsyncLog();
    ~AsyncLog();

    void start();

    void append(const char *msg, int size);

};    // class AsyncLog

}   // namesapce YTalk

#endif