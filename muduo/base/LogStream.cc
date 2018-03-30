/*
 * LogStream.cc
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

#include <muduo/base/LogStream.h>

#include <algorithm>
#include <limits>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

using namespace muduo;
using namespace muduo::detail;

#pragma GCC diagnostic ignored "-Wtype-limits"

namespace muduo
{
namespace detail
{
const char digits[] = "9876543210123456789";
const char *zero    = digits+9;
BOOST_STATIC_ASSERT(sizeof(digits) == 20);

const char digitsHex[] = "0123456789ABCDEF";
BOOST_STATIC_ASSERT(sizeof(digitsHex) == 17);

template<typename T>
size_t convert(char buf[], T value)
{
    T i     = value;
    char *p = buf;
    do {
        int lsd = static_cast<int>(i % 10);
        i       /= 10;
        *p++    = zero[lsd];
    } while(i != 0);

    if (value < 0) { *p++ = '-'; }
    *p = '\0';

    std::reverse(buf, p);
    return p - buf;
}

size_t convertHex(char buf[], uintptr_t value)
{
    uintptr_t i = value;
    char *p     = buf;
    do {
        int lsd = i % 16;
        i       /= 16;
        *p++    = digitsHex[lsd];
    } while ( i != 0);
    
    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}

} // detail

} // muduo

template <int SIZE>
const char *FixedBuffer<SIZE>::debugString()
{
    *cur_ = '\0';
    return data_;
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieStart()
{}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd()
{}

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;

void LogStream::staticCheck()
{
    BOOST_STATIC_ASSERT(kMaxNumbericSize - 10 > 
            std::numeric_limits<double>::digits10);
    BOOST_STATIC_ASSERT(kMaxNumbericSize - 10 > 
            std::numeric_limits<long double>::digits10);
    BOOST_STATIC_ASSERT(kMaxNumbericSize - 10 > 
            std::numeric_limits<long>::digits10);
    BOOST_STATIC_ASSERT(kMaxNumbericSize - 10 > 
            std::numeric_limits<long long>::digits10);
}

template <typename T>
void LogStream::formatInteger(T v)
{
    if (buffer_.avail() >= kMaxNumbericSize) {
        size_t len = convert(buffer_.current(), v); 
        buffer_.add(len);
    }
}

LogStream& LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(const void *p)
{
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    if (buffer_.avail() >= kMaxNumbericSize) {
        char *buf = buffer_.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf+2, v);
        buffer_.add(len+2);
    }
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    if (buffer_.avail() >= kMaxNumbericSize) {
        int len = snprintf(buffer_.current(), kMaxNumbericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}

template<typename T>
Fmt::Fmt(const char *fmt, T val)
{
    BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value == true);

    length_ = snprintf(buf_, sizeof buf_, fmt, val);
    assert(static_cast<size_t>(length_) < sizeof buf_);
}

// explicit instantiations
template Fmt::Fmt(const char *fmt, char);

template Fmt::Fmt(const char *fmt, short);
template Fmt::Fmt(const char *fmt, unsigned short);
template Fmt::Fmt(const char *fmt, int);
template Fmt::Fmt(const char *fmt, unsigned int);
template Fmt::Fmt(const char *fmt, long);
template Fmt::Fmt(const char *fmt, unsigned long);
template Fmt::Fmt(const char *fmt, long long);
template Fmt::Fmt(const char *fmt, unsigned long long);
template Fmt::Fmt(const char *fmt, float);
template Fmt::Fmt(const char *fmt, double);
