/*================================================================================   
 *    Date: 2021-11-10
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#ifndef YTALK_TIME_H
#define YTALK_TIME_H

#include <butil/time/time.h>

#include <string>

namespace YTalk
{

class Time
{
public:
    Time();
    Time(const Time &that);
    ~Time();

    static Time now();

    std::string timeToStringUTC() const;
    std::string timeToStringLocal() const;

private:
    Time(const butil::Time &that);

    butil::Time _time;

};    // class Time

}    // namesapce YTalk

#endif

