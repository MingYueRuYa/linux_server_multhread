/*
 * ThreadPool.h
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

#ifndef MUDUO_BASE_THREADPOOL_H
#define MUDUO_BASE_THREADPOOL_H

#include <muduo/base/Condition.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/Thread.h>
#include <muduo/base/Types.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <deque>

namespace muduo
{
class ThreadPool : boost::noncopyable
{
public:
    typedef boost::function<void ()> Task;

    explicit ThreadPool(const string &name = string());
    ~ThreadPool();
    
    void start(int numThreads);
    void stop();

    void run(const Task& f);

private:
    void runInThread();
    Task take();

private:
    MutexLock   mutex_;
    Condition   cond_;
    string      name_;
    bool        running_;
    boost::ptr_vector<muduo::Thread> threads_;
    std::deque<Task> queue_;
};
}

#endif // MUDUO_BASE_THREADPOOL_H
