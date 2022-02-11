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

#ifndef YTALK_TIME_STAMP_H
#define YTALK_TIME_STAMP_H

#include <stdint.h>
#include <algorithm>
#include <string>

namespace YTalk
{

namespace base
{

class TimeStamp
{
public:
	TimeStamp() : microSecondsSinceEpoch_(0) {
		//TODO
	}

	explicit TimeStamp(int64_t microSecondsSinceEpoch);

    TimeStamp& operator+=(TimeStamp lhs) {
        this->microSecondsSinceEpoch_ += lhs.microSecondsSinceEpoch_;
        return *this;
    }

    TimeStamp& operator+=(int64_t lhs) {
        this->microSecondsSinceEpoch_ += lhs;
        return *this;
    }

    TimeStamp& operator-=(TimeStamp lhs) {
        this->microSecondsSinceEpoch_ -= lhs.microSecondsSinceEpoch_;
        return *this;
    }

	TimeStamp& operator-=(int64_t lhs) {
        this->microSecondsSinceEpoch_ -= lhs;
        return *this;
    }

	void swap(TimeStamp& that) {
		std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
	}

	std::string toString() const;
	std::string toFormattedString(bool showMicroseconds = true) const;

	bool valid() const { return microSecondsSinceEpoch_ > 0; }

	int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
	time_t secondsSinceEpoch() const {
		return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
	}

	static TimeStamp now();
	static TimeStamp invalid();

	static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
	int64_t     microSecondsSinceEpoch_;
};

inline bool operator<(TimeStamp lhs, TimeStamp rhs) {
	return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator>(TimeStamp lhs, TimeStamp rhs) {
	return rhs < lhs;
}

inline bool operator<=(TimeStamp lhs, TimeStamp rhs) {
	return !(lhs > rhs);
}

inline bool operator>=(TimeStamp lhs, TimeStamp rhs) {
	return !(lhs < rhs);
}

inline bool operator==(TimeStamp lhs, TimeStamp rhs) {
	return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline bool operator!=(TimeStamp lhs, TimeStamp rhs) {
	return !(lhs == rhs);
}

inline double timeDifference(TimeStamp high, TimeStamp low) {
	int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
	return static_cast<double>(diff) / TimeStamp::kMicroSecondsPerSecond;
}

inline TimeStamp addTime(TimeStamp timestamp, int64_t microseconds) {
	return TimeStamp(timestamp.microSecondsSinceEpoch() + microseconds);
}

} //namespace base

}   // namespace YTalk

#endif   // YTALK_TIME_STAMP_H