/*
 * FileUtil.cc
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

#include <muduo/base/LogFile.h>
#include <muduo/base/Logging.h> //strerror_tl
#include <muduo/base/ProcessInfo.h>

#include <assert.h>
#include <stdio.h>
#include <time.h>

using namespace muduo;

// not thread safe
class LogFile::File : boost::noncopyable
{
public:
    explicit File(const string &filename)
        : fp_(::fopen(filename.data(), "ae"))
    {
        assert(fp_);
        ::setbuffer(fp_, buffer_, sizeof buffer_);
    }
    
    ~File() { ::fclose(fp_); }

    void append(const char *logline, const size_t len)
    {
        size_t n        = write(logline, len);
        size_t remain   = len - n;
        //remain > 0表示没有写完，需要继续写完直到写完
        while (remain > 0) {
            size_t x = write(logline+n, remain);
            if (0 == x) {
                int err = ferror(fp_);
                if (err) {
                    fprintf(stderr, "LogFile::File::append() failed %s\n",
                            strerror_tl(err));
                } // if
                break;
            } // if
            n += x;
            remain = len - n;
        } // while
        writtenBytes_ += len;
    }

    void flush()
    {
        ::fflush(fp_);
    }

    size_t writeenBytes() const { return writtenBytes_; }

private:
    size_t write(const char *logline, size_t len)
    {
#undef fwrite_unlocked        
        return ::fwrite_unlocked(logline, 1, len, fp_);
    }

private:
    FILE *fp_;
    char buffer_[64*1024];
    size_t writtenBytes_;
};

LogFile::LogFile(const string &basename, size_t rollSize,
                 bool threadSafe,
                 int flushInterval)
    : basename_(basename),
      rollSize_(rollSize),
      flushInterval_(flushInterval),
      mutex_(threadSafe ? new MutexLock : NULL),
      startOfPeriod_(0),
      lastRoll_(0),
      lastFlush_(0)
{
    assert(basename.find('/') == string::npos);
    rollFile();
}

LogFile::~LogFile()
{}

void LogFile::append(const char *logline, int len)
{
    if (mutex_) {
        MutexLockGuard lock(*mutex_);
        append_unlocked(logline, len);
    } else {
        append_unlocked(logline, len);
    }
}

void LogFile::flush()
{
    if (mutex_) {
        MutexLockGuard lock(*mutex_);
        file_->flush();
    } else {
        file_->flush();
    }
}

void LogFile::append_unlocked(const char *logline, int len)
{
    file_->append(logline, len);
    
    if (file_->writeenBytes() > rollSize_) {
        rollFile();
    } else {
        if (count_ > kCheckTimeRoll_) {
           count_ = 0; 
           time_t now = ::time(NULL);
           time_t thisPeriod = now / kRollPerSeconds_ * kRollPerSeconds_;
           if (thisPeriod != startOfPeriod_) {
                rollFile();
           } else if (now - lastFlush_ > flushInterval_) {
                lastFlush_ = now;
                file_->flush();
           }
        } else {
            ++count_;
        }
    }
}

void LogFile::rollFile()
{
    time_t now = 0;    
    string filename = getLogFileName(basename_, &now);
    // 注意，这里先除kRollPerSeconds_ 后乘kRollPerSeconds_ 表示
    // 对齐至kRollPerSeconds_整数倍，也就是时间调整到当天零点
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;
    
    if (now > lastRoll_) {
        lastRoll_   = now;
        lastFlush_  = now;
        startOfPeriod_ = start;
        file_.reset(new File(filename));
    }
}

string LogFile::getLogFileName(const string &basename, time_t *now)
{
    string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32]    = {0};
    char pidbuf[32]     = {0};
    
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;
    filename += ProcessInfo::hostname();
    snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
    filename += pidbuf;
    filename += ".log";
    return filename;
}
