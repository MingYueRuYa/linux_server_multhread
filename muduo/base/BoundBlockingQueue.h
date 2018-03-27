/*
 * BoundBlockingQueue.h
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

#ifndef MUDUO_BASE_BOUNDBLOCKINGQUEUE_H
#define MUDUO_BASE_BOUNDBLOCKINGQUEUE_H

#include <muduo/base/Condition.h>
#include <muduo/base/Mutex.h>

#include <boost/circular_buffer.hpp>
#include <boost/noncopyable.hpp>
#include <assert.h>

namespace muduo
{
template<typename T>
class BoundBlockingQueue : boost::noncopyable
{
public:
    BoundBlockingQueue(int maxSize) 
        : mutex_(),
          notEmpty_(mutex_),
          notFull_(mutex_),
          queue_(maxSize)
    {}

    void put(const T &x)
    {
        MutexLockGuard lock(mutex_);
        while (queue_.full()) {
            notFull_.wait();
        }
        
        assert(! queue_.full());
        queue_.push_back(x);
        notEmpty_.notify();
    }

    T take()
    {
        MutexLockGuard lock(mutex_);
        while (queue_.empty()) {
            notEmpty_.wait();
        }
        
        assert(! queue_.empty());
        T front(queue_.front());
        queue_.pop_front();
        notFull_.notify();
        return front;
    }

    bool empty() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.empty();
    }

    bool full() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.full();
    }

    int size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

    int capacity() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.capacity();
    }

private:
    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    boost::circular_buffer<T> queue_;
};
}

#endif // MUDUO_BASE_BOUNDBLOCKINGQUEUE_H
