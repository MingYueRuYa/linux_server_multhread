/*
 * ThreadPool.cc
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

#include <muduo/base/ThreadPool.h>
#include <muduo/base/Exception.h>

#include <boost/bind.hpp>
#include <assert.h>
#include <stdio.h>

using namespace muduo;

ThreadPool::ThreadPool(const string &name)
    : mutex_(),
      cond_(mutex_),
      name_(name),
      running_(false)
{}

ThreadPool::~ThreadPool()
{
    if (! running_) {
        return; 
    }

    stop();
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);
    for (int i=0; i<numThreads; ++i) {
        char id[32] = {0};
        snprintf(id, sizeof id, "%d", i);
        threads_.push_back(new muduo::Thread(
                    boost::bind(&ThreadPool::runInThread, this), name_+id));
        threads_[i].start();
    }
}

void ThreadPool::stop()
{
    {
        MutexLockGuard lock(mutex_);
        running_ = false;
        cond_.notifyAll();
    }
    
    for_each(threads_.begin(), threads_.end(), 
            boost::bind(&muduo::Thread::join, _1));
}

void ThreadPool::run(const Task &task)
{
    // 当队列为空时，在当前调用的线程执行task???
    if (threads_.empty()) {
        task();
    } else {
        MutexLockGuard lock(mutex_);
        queue_.push_back(task);
        cond_.notify();
    }
}

ThreadPool::Task ThreadPool::take()
{
    MutexLockGuard lock(mutex_);

    while (queue_.empty() && running_) {
        cond_.wait();
    }

    Task task;
    // 这里进行empty判断是最后stop时候,cond_.notifyAll()通知所有的线程结束
    // 不在等待
    if (! queue_.empty()) {
        task = queue_.front();
        queue_.pop_front();
    }
    return task;
}

void ThreadPool::runInThread()
{
    try {
        // 多线程调用
        while (running_) {
            Task task(take());
            if (task) {
                task();
            }
        }
    } catch (const Exception &ex) {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason:%s\n", ex.what());
        fprintf(stderr, "stack trace:%s\n", ex.stackTrace());
    } catch (const std::exception &ex) {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason:%s\n", ex.what());
    } catch (...) {
        fprintf(stderr, "unknown, exception caught in ThreadPool %s\n"
                , name_.c_str());
        throw;
    }
}
