/*
 * Thread_unittest.cc
 * Copyright (c) 2018 liu <714049438@qq.com>
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

#include <string>
#include <boost/bind.hpp>
#include <stdio.h>

void threadFunc()
{
    printf("tid=%d\n", muduo::CurrentThread::tid());
}

void threadFunc2(int x)
{
    printf("tid=%d, x=%d, name=%s\n", muduo::CurrentThread::tid(), x, 
                                      muduo::CurrentThread::name());
}

class Foo
{
public:    
    explicit Foo(double x)
        : x_(x)
    {}

    void memberFunc()
    { printf("tid=%d, Foo::x=%f\n", muduo::CurrentThread::tid(), x_); }

    void memberFunc2(const std::string &text)
    {
        printf("tid=%d, Foo::x_=%f, text=%s\n", muduo::CurrentThread::tid(),
                x_, text.c_str());
    }
private:
    double x_;
};

int main(int argc, char *argv[])
{
    printf("pid=%d, tid=%d, name=%s\n", ::getpid(), 
                muduo::CurrentThread::tid(), muduo::CurrentThread::name());

    muduo::Thread t1(threadFunc);
    t1.start();
    t1.join();

    muduo::Thread t2(boost::bind(threadFunc2, 42), 
                    "thread for free funtion with argument");
    t2.start();
    t2.join();

    Foo foo(87.53);
    muduo::Thread t3(boost::bind(&Foo::memberFunc, &foo),
                    "thread for member function without argument");
    t3.start();
    t3.join();

    muduo::Thread t4(boost::bind(&Foo::memberFunc2, boost::ref(foo),
                     ("liushixiong")));
    t4.start();
    t4.join();

    printf("number of created threads %d\n", muduo::Thread::numCreated());

    return 0;
}

