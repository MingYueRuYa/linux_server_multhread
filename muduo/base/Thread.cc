/*
 * Thread.cc
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

#include <muduo/base/Thread.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Exception.h>
// #include <muduo/base/Loggin.h>

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace muduo
{
namespace CurrentThread
{
    // __thread 修饰的变量是线程局部存储的
    // 线程真实pid(tid)的缓存
    // 是为了减少::syscall(SYS_gettid) 系统调用的次数
    // 提高获取tid的效率
    __thread int t_cachedTid = 0;
    // 这是tid的字符串表示形式
    __thread char t_tidString[32];
    __thread const char *t_threadName = "unkown";
    const bool sameType = boost::is_same<int, pid_t>::value;
    BOOST_STATIC_ASSERT(sameType);
}

namespace detail
{
    pid_t gettid()
    { return static_cast<pid_t>(::syscall(SYS_gettid)); }

    void afterFork()
    {
        muduo::CurrentThread::t_cachedTid   = 0;
        muduo::CurrentThread::t_threadName  = "main";
        CurrentThread::tid();
    }

class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        muduo::CurrentThread::t_threadName = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

// global variable
ThreadNameInitializer init;
    
}
}

using namespace muduo;

void CurrentThread::cacheTid()
{
    if (0 != t_cachedTid) {
        return;
    }

    t_cachedTid = detail::gettid();
    int n = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
    assert(n == 6); (void)n;
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const string &n)
    : started_(false),
      pthreadId_(0),
      tid_(0),
      func_(func),
      name_(n)
{
    numCreated_.increment();
}

Thread::~Thread()
{}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    errno = pthread_create(&pthreadId_, NULL, &startThread, this);
    if (0 != errno) {
        //LOG_SYSFATAL << "Failed in pthread_create";
    }
}

int Thread::join()
{
    assert(started_);
    return pthread_join(pthreadId_, NULL);
}

void *Thread::startThread(void *obj)
{
    Thread *thread = static_cast<Thread *>(obj);
    thread->runInThread();
    return NULL;
}

void Thread::runInThread()
{
    tid_ = CurrentThread::tid();
    muduo::CurrentThread::t_threadName = name_.c_str();
    try {
        func_();
        muduo::CurrentThread::t_threadName = "finished";
    } catch (const Exception &ex) {
        muduo::CurrentThread::t_threadName = "crashed";
        fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
        abort();
    } catch (const std::exception &ex) {
        muduo::CurrentThread::t_threadName = "crashed";
        fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
        fprintf(stderr, "reason:%s\n", ex.what());
        abort();
    } catch (...) {
        muduo::CurrentThread::t_threadName = "crashed";
        fprintf(stderr, "unkown exception caught in Thread %s\n", 
                name_.c_str());
        throw;  //继续传递异常
    }
}
