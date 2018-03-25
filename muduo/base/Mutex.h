/*
 * Mutex.h
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

#ifndef MUDUO_BASE_MUTEX_H
#define MUDUO_BASE_MUTEX_H

#include <muduo/base/CurrentThread.h>
#include <boost/noncopyable.hpp>
#include <assert.h>
#include <pthread.h>

namespace muduo
{
class MutexLock : public boost::noncopyable
{
public:
    MutexLock()
        : holder_(0)
    {
        int ret = pthread_mutex_init(&mutex_, NULL);
        assert(ret == 0); (void) ret;
    }

    ~MutexLock()
    {
        assert(holder_ == 0);
        int ret = pthread_mutex_destroy(&mutex_);
        assert(ret == 0); (void) ret;
    }

    bool isLockedbyThisThread()
    {
        return holder_ == CurrentThread::tid();
    }

    void assertLocked()
    {
        assert(isLockedbyThisThread());
    }

    void lock()
    {
        pthread_mutex_lock(&mutex_);
        holder_ = CurrentThread::tid();
    }

    void unlock()
    {
        holder_ = 0;
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* getPthreadMutex() /* non-const */
    {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
    pid_t holder_;

};

class MutexLockGuard : boost::noncopyable
{
public:
    explicit MutexLockGuard(MutexLock &mutex)
        :mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    { mutex_.unlock(); }

private:
    MutexLock &mutex_; 
};

}

#endif // MUDUO_BASE_MUTEX_H
