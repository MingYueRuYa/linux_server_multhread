/*
 * SingletonThreadLocal_test.cc
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

#include <muduo/base/Singleton.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/ThreadLocal.h>
#include <muduo/base/ThreadLocalSingleton.h>
#include <muduo/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <stdio.h>

class Test : boost::noncopyable
{
public:
    Test()
    { printf("tid=%d, constructing %p\n", muduo::CurrentThread::tid(), this); }

    ~Test()
    { printf("tid=%d, destructing %p %s\n", muduo::CurrentThread::tid(), 
            this, name_.c_str()); }

    const std::string &name() const { return name_; }
    void setName(const std::string &name) { name_ = name; }

private:
    std::string name_;
};

void threadFunc(const char *changeTo)
{
    printf("tid=%d, %p name=%s\n",
            muduo::CurrentThread::tid(),
            &muduo::ThreadLocalSingleton<Test>::instance(),
            muduo::ThreadLocalSingleton<Test>::instance().name().c_str());

    muduo::ThreadLocalSingleton<Test>::instance().setName(changeTo);

    printf("tid=%d, %p name=%s\n",
            muduo::CurrentThread::tid(),
            &muduo::ThreadLocalSingleton<Test>::instance(),
            muduo::ThreadLocalSingleton<Test>::instance().name().c_str());
}

int main()
{
    muduo::ThreadLocalSingleton<Test>::instance().setName("main one");

    muduo::Thread t1(boost::bind(threadFunc, "thread1"));
    muduo::Thread t2(boost::bind(threadFunc, "thread2"));
    t1.start();
    t2.start();
    t1.join();

    printf("tid=%d, %p name=%s\n",
            muduo::CurrentThread::tid(),
            &muduo::ThreadLocalSingleton<Test>::instance(),
            muduo::ThreadLocalSingleton<Test>::instance().name().c_str());

    t2.join();

    pthread_exit(0);
}

