/*
 * Logging.h
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

#ifndef MUDUO_BASE_LOGGING_H
#define MUDUO_BASE_LOGGING_H

#include <muduo/base/LogStream.h>
#include <muduo/base/Timestamp.h>

namespace muduo
{

class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS
    };

    // compile time calculation of basename of source file
    class SourceFile
    {
    public:
        template<int N>
        inline SourceFile(const char (&arr)[N])
                    : data_(arr),
                      size_(N-1)
        {
            const char *slash = strrchr(data_, '/');
            if (slash) {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char *filename)
            : data_(filename)
        {
            const char *slash = strrchr(data_, '/');
            if (slash) {
                data_ = slash + 1;
            }
            size_ = static_cast<size_t>(strlen(data_));
        }

    public:
        const char *data_;
        int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char *func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    typedef void (*OutputFunc)(const char *msg, int len);
    typedef void (*FlushFunc)();
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);

private:
    class Impl
    {
    public:
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
        void formatTime();
        void finish();

    public:
        Timestamp   time_;
        LogStream   stream_;
        LogLevel    level_;
        int         line_;
        SourceFile  basename_;
    };

    Impl impl_;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}

#define LOG_TRACE if (muduo::Logger::logLevel() <= muduo::Logger::TRACE)  \
    muduo::Logger(__FILE__, __LINE__, muduo::Logger::TRACE, __func__).stream()

#define LOG_DEBUG if (muduo::Logger::logLevel() <= muduo::Logger::DEBUG)  \
    muduo::Logger(__FILE__, __LINE__, muduo::Logger::DEBUG, __func__).stream()

#define LOG_INFO if (muduo::Logger::logLevel() <= muduo::Logger::INFO)  \
    muduo::Logger(__FILE__, __LINE__).stream()

#define LOG_WARN muduo::Logger(__FILE__, __LINE__   \
        , muduo::Logger::WARN).stream()

#define LOG_ERROR muduo::Logger(__FILE__, __LINE__   \
        , muduo::Logger::ERROR).stream()

#define LOG_FATAL muduo::Logger(__FILE__, __LINE__   \
        , muduo::Logger::FATAL).stream()

#define LOG_SYSERR muduo::Logger(__FILE__, __LINE__, false).stream()

#define LOG_SYSFATAL muduo::Logger(__FILE__, __LINE__, true).stream()

const char *strerror_tl(int savedErrno);

#define CHECK_NOTNULL(val)  \
    ::muduo::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL" \
            , (val) )

template <typename T>
T *CheckNotNull(Logger::SourceFile file, int line, const char *names, T *ptr)
{
    if (NULL == ptr) {
        Logger(file, line, Logger::FATAL).stream() << names;
    }
    return ptr;
}

} // muduo

#endif // MUDUO_BASE_LOGGING_H
