/*
 * Thread.h
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

#ifndef MUDUO_BASE_THREAD_H
#define MUDUO_BASE_THREAD_H

#include <muduo/base/Atomic.h>
#include <muduo/base/Types.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace muduo
{
class Thread : public boost::noncopyable
{
public:
    typedef boost::function<void ()> ThreadFunc;

    explicit Thread(const ThreadFunc &, const string &name = string());
    ~Thread();

    void start();
    int join(); // return pthread_join()

    bool started() const { return started_; } 

    //pthread_t pthreadId() const { return pthreadId_; }
    pid_t tid() const { return tid_; }
    const string &name() const { return name_; }

    static int numCreated() { return numCreated_.get(); }

private:
    static void *startThread(void *thread);
    void runInThread();

private:
    static AtomicInt32 numCreated_;

private:
    bool        started_;
    pthread_t   pthreadId_;
    pid_t       tid_;
    ThreadFunc  func_;
    string      name_;

};
}

#endif //MUDUO_BASE_THREAD_H

