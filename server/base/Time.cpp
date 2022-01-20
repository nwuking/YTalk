/*================================================================================   
 *    Date: 2021-11-10
 *    Author: nwuking
 *    Email: nwuking@qq.com  
================================================================================*/

#include "./Time.h"

namespace YTalk
{

Time::Time() : _time() {
    //
}

Time::Time(const Time &that) : _time(that._time) {
    //
}

Time::Time(const butil::Time &that) : _time(that) {
    //private
}

Time::~Time() {
    //
}

Time Time::now() {
    return Time(butil::Time::Now());
}

std::string Time::timeToStringUTC() const {
    butil::Time::Exploded exp;
    ::bzero(&exp, sizeof(exp));
    _time.UTCExplode(&exp);
    
    char strtmp[64];
    snprintf(strtmp, sizeof(strtmp), "%d-%d-%d %d:%d:%d.%d", exp.year,
                                         exp.month,
                                         exp.day_of_month,
                                         exp.hour,
                                         exp.minute,
                                         exp.second,
                                         exp.millisecond);

    return strtmp;
}

std::string Time::timeToStringLocal() const {
    butil::Time::Exploded exp;
    ::bzero(&exp, sizeof(exp));
    _time.LocalExplode(&exp);
    
    char strtmp[64];
    snprintf(strtmp, sizeof(strtmp), "%d-%d-%d %d:%d:%d.%d", exp.year,
                                         exp.month,
                                         exp.day_of_month,
                                         exp.hour,
                                         exp.minute,
                                         exp.second,
                                         exp.millisecond);

    return strtmp;
}



}    // namespace YTalk