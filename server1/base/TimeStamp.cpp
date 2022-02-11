/**
 * @file TimeStamp.h
 * @author nwuking@qq.com
 * @brief 
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "TimeStamp.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdio.h>

namespace YTalk
{

namespace base
{

static_assert(sizeof(TimeStamp) == sizeof(int64_t), "sizeof(TimeStamp) error");

TimeStamp::TimeStamp(int64_t microSecondsSinceEpoch)
: microSecondsSinceEpoch_(microSecondsSinceEpoch)
{
}

std::string TimeStamp::toString() const
{
    char buf[64] = { 0 };
    int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
    snprintf(buf, sizeof(buf)-1, "%lld.%06lld", (long long int)seconds, (long long int)microseconds);
    return buf;
}

std::string TimeStamp::toFormattedString(bool showMicroseconds) const
{
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
	struct tm tm_time;

	struct tm *ptm;
	ptm = localtime(&seconds);
	tm_time = *ptm;

	char buf[32] = { 0 };

	if (showMicroseconds)
	{
		int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);

		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
			microseconds);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	}
	
	
	return buf;
}

TimeStamp TimeStamp::now()
{
	std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> now = std::chrono::time_point_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now());

	int64_t microSeconds = now.time_since_epoch().count();
	TimeStamp time(microSeconds);
	return time;
}

TimeStamp TimeStamp::invalid()
{
	return TimeStamp();
}

}  //namespace base

}  // namespace YTalk