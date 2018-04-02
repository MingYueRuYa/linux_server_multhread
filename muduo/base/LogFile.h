/*
 * LogFile.h
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

#ifndef MUDUO_BASE_LOGFILE_H

#include <muduo/base/Mutex.h>
#include <muduo/base/Types.h>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo
{
class LogFile : boost::noncopyable
{
public:
    LogFile(const string &basename,
            size_t rollSize,
            bool threadSafe = true,
            int flushInterval = 3);

    ~LogFile();

    void append(const char *logline, int len);
    void flush();

private:
    void append_unlocked(const char *logline, int len);
    void rollFile();

    static string getLogFileName(const string &basename, time_t *now);

private:
    const string basename_; // 日志文件名
    const size_t rollSize_; // 文件达到rollSize 换一个新文件
    const int flushInterval_;   // 日志写入间隔时间

    int count_;
    boost::scoped_ptr<MutexLock> mutex_;
    time_t startOfPeriod_;  // 开始记录日志时间
    time_t lastRoll_;       // 上一次滚动日志文件时间
    time_t lastFlush_;      // 上一次日志写入时间
    
    class File;
    boost::scoped_ptr<File> file_;
    const static int kCheckTimeRoll_    = 1024;
    const static int kRollPerSeconds_   = 60*60*24;
};
} // muduo

#endif // MUDUO_BASE_LOGFILE_H

