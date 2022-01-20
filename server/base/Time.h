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

typedef struct {
    int year;
    int month;
    int day_of_week;
    int day_of_month;
    int hour;
    int minute;
    int second;
    int millisecond;
} Exploded;

class Time
{
public:
    Time();
    Time(const Time &that);
    ~Time();

    static Time now();

    std::string timeToStringUTC() const;
    std::string timeToStringLocal() const;

    //Time addTime(const Exploded &exploded);

private:
    Time(const butil::Time &that);

    butil::Time _time;

};    // class Time

}    // namesapce YTalk

#endif

