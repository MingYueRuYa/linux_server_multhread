/*
 * Copyright (c) 2018 liu <714049438@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MUDUO_TIMESTAMP_H
#define MUDUO_TIMESTAMP_H

#include <muduo/base/copyable.h>
#include <muduo/base/Types.h>
#include <boost/operators.hpp>

namespace muduo
{
///
/// Time stamp in UTC, in microseconds resolution.
///
/// This class is immutable.
/// It's recommended to pass it by value, since it's passed in register on x64.
///
class Timestamp : public muduo::copyable,
                  public boost::less_than_comparable<Timestamp> 

{
public:
    Timestamp() : microSecondsSinceEpoch_(0) 
    {}
    
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    
    // default copy/assignment/dtor are Okay

    void swap(Timestamp &that)
    {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }

    string toString() const;
    string toFormattedString() const;

    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    time_t secondsSinceEpoch() const
    { 
        return static_cast<time_t>  \
            (microSecondsSinceEpoch_ / kMicroSecondPerSecond);
    }

    static Timestamp now();
    static Timestamp invalid();

    static const int kMicroSecondPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeDifference(Timestamp high, Timestamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() \
                    - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondPerSecond;
}

// 为什么不变成一个static成员函数?
inline Timestamp addTime(Timestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t> \
        (seconds * Timestamp::kMicroSecondPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}
#endif // MUDUO_TIMESTAMP_H
