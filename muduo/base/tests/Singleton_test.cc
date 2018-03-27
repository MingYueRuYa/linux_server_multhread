/*
 * Singleton_test.cc
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
#include <muduo/base/Thread.h>

#include <boost/noncopyable.hpp>
#include <stdio.h>

class Test : boost::noncopyable
{
public:
    Test()
    {
        printf("tid=%d, constructing %p \n", muduo::CurrentThread::tid(), this);
    }

    ~Test()
    {
        printf("tid=%d, destructing %p %s\n", 
                muduo::CurrentThread::tid(), this, name_.c_str());
    }

    const muduo::string &name() const { return name_; }
    void setName(const muduo::string &name) { name_ = name; }

private:
    muduo::string name_;
};

void ThreadFunc()
{
    printf("tid=%d, %p name = %s\n",
            muduo::CurrentThread::tid(),
            &muduo::Singleton<Test>::instance(),
            muduo::Singleton<Test>::instance().name().c_str());
    muduo::Singleton<Test>::instance().setName("only one, changed");
}

int main()
{
    muduo::Singleton<Test>::instance().setName("only one");
    muduo::Thread t1(ThreadFunc);
    t1.start();
    t1.join();
    printf("tid=%d, %p name = %s\n",
            muduo::CurrentThread::tid(),
            &muduo::Singleton<Test>::instance(),
            muduo::Singleton<Test>::instance().name().c_str());
    return 0;
}
