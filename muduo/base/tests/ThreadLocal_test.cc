/*
 * ThreadLocal_test.cc
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

#include <muduo/base/ThreadLocal.h>
#include <muduo/base/Thread.h>
#include <muduo/base/CurrentThread.h>

#include <boost/noncopyable.hpp>
#include <stdio.h>

class Test : boost::noncopyable
{
public:
    Test()
    { printf("tid=%d, constructing %p\n", muduo::CurrentThread::tid(), this); }

    ~Test()
    { printf("tid=%d, destrcuting %p %s \n", 
            muduo::CurrentThread::tid(),
            this,
            name_.c_str()); }

    const std::string name() const { return name_; }
    void setName(const std::string &name) { name_ = name; }

private:
    std::string name_;
};

muduo::ThreadLocal<Test> testObj1;
muduo::ThreadLocal<Test> testObj2;

void print()
{
    printf("tid=%d, obj1 %p name=%s\n",
            muduo::CurrentThread::tid(),
            &testObj1.value(),
            testObj1.value().name().c_str());

    printf("tid=%d, obj2 %p name=%s\n",
            muduo::CurrentThread::tid(),
            &testObj2.value(),
            testObj2.value().name().c_str());
}

void threadFunc()
{
    print();

    testObj1.value().setName("changed 1");
    testObj2.value().setName("changed 42");

    print();
}

int main()
{
    testObj1.value().setName("main one");

    print();

    muduo::Thread t1(threadFunc);
    t1.start();
    t1.join();

    testObj2.value().setName("main two");

    print();
    pthread_exit(0);
}
