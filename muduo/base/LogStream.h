/*
 * LogStream.h
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

#ifndef MUDUO_BASE_LOGSTREAM_H
#define MUDUO_BASE_LOGSTREAM_H

#include <muduo/base/StringPiece.h>
#include <muduo/base/Types.h>
#include <assert.h>
#include <string.h>

#ifndef MUDUO_STD_STRING
#include <string>
#endif // MUDUO_STD_STRING

#include <boost/noncopyable.hpp>

namespace muduo
{
namespace detail
{
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

template<int SIZE>
class FixedBuffer : boost::noncopyable
{
public:
    FixedBuffer()
        : cur_(data_)
    {
        setCookie(cookieStart);
    }

    ~FixedBuffer() { setCookie(cookieEnd); }

    void append(const char *buf, size_t len)
    {
        if (implicit_cast<size_t>(avail()) > len) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char *data() const { return data_; }
    int length() const { return static_cast<int>(cur_ - data_); }

    char *current() { return cur_; }
    int avail() const { return static_cast<int>(end() - cur_); }
    void add(size_t len) { cur_ += len; }

    void reset() { cur_ = data_; }
    void bzero() { ::bzero(data_, sizeof data_); }

    // for used GDB
    const char *debugString();

    void setCookie(void (*cookie)()) { cookie_ = cookie; }
    // for used unit test
    string asString() { return string(data_, length()); }

private:
    const char *end() const { return data_+ sizeof data_;}
    static void cookieStart();
    static void cookieEnd();

private:
    void (*cookie_)();
    char data_[SIZE];
    char *cur_;
};

}   // detail

class LogStream : boost::noncopyable
{
typedef LogStream self;
public:
    typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

    self &operator <<(bool v)
    {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }

    self &operator<<(short);
    self &operator<<(unsigned short);
    self &operator<<(int);
    self &operator<<(unsigned int);
    self &operator<<(long);
    self &operator<<(unsigned long);
    self &operator<<(long long);
    self &operator<<(unsigned long long);

    self &operator<<(const void *);

    self &operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }

    self &operator<<(double v);

    self &operator<<(char v)
    {
        buffer_.append(&v, 1);
        return *this;
    }

    self &operator<<(const char *v)
    {
        buffer_.append(v, strlen(v));
        return *this;
    }

    self &operator<<(const string &v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }

#ifdef MUDUO_STD_STRING
    self &operator<<(const std::string &v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }
#endif // MUDUO_STD_STRING

    self &operator<<(const StringPiece &v)
    {
        buffer_.append(v.data(), v.size());
        return *this;
    }

    void append(const char *data, int len) { buffer_.append(data, len); }
    const Buffer& buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

private:
    void staticCheck();

    template <typename T>
    void formatInteger(T);

private:
    Buffer buffer_;
    static const int kMaxNumbericSize = 32;
};

class Fmt
{
public:
    template <typename T>
    Fmt(const char *fmt, T val);

    const char *data() const { return buf_; }
    int length() const { return length_; }

private:
    char buf_[32];
    int length_;
};

inline LogStream& operator<<(LogStream &s, const Fmt &fmt)
{
    s.append(fmt.data(), fmt.length());
    return s;
}

} // muduo

#endif // MUDUO_BASE_LOGSTREAM_H
