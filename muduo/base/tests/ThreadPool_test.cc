/*
 * ThreadPool_test.cc
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
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/CurrentThread.h>

#include <boost/bind.hpp>
#include <stdio.h>

void print()
{
    printf("tid=%d\n", muduo::CurrentThread::tid());
}

void printString(const std::string &str)
{
    printf("tid=%d, str=%s\n", muduo::CurrentThread::tid(), str.c_str());
}

int main()
{
    muduo::ThreadPool pool("MainThreadPool");
    pool.start(5);
    pool.run(print);
    pool.run(print);

    for (int i=0; i<100; ++i) {
        char buf[32] = {0};
        snprintf(buf, sizeof buf, "task %d", i);
        pool.run(boost::bind(printString, std::string(buf)));
    }

    muduo::CountDownLatch latch(1);
    pool.run(boost::bind(&muduo::CountDownLatch::countDown, &latch));
    latch.wait();
    pool.stop();
    return 0;
}

