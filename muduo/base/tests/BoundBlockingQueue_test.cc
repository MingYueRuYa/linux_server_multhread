/*
 * BlocingQueue_text.cc
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

#include <muduo/base/BoundBlockingQueue.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <stdio.h>

class Test
{
public:
    Test(int numThreads)
        : latch_(numThreads),
          threads_(numThreads),
          queue_(20)
    {
        for (int i=0; i<numThreads; ++i) {
            char name[32] = {0};
            snprintf(name, sizeof name, "work thread %d", i);
            threads_.push_back(new muduo::Thread(
                        boost::bind(&Test::threadFunc, this), 
                        muduo::string(name)));
        }
        for_each(threads_.begin(), threads_.end(), 
                boost::bind(&muduo::Thread::start, _1));
    }

    void run(int times)
    {
        printf("waiting for count down latch\n");
        latch_.wait();
        printf("all threads started\n");
        for (int i=0; i<times; ++i) {
            char buf[32] = {0};
            snprintf(buf, sizeof buf, "hello %d", i);
            queue_.put(buf);
            printf("tid=%d, put data = %s, size = %d\n", 
                    muduo::CurrentThread::tid(), buf, queue_.size());
        }
    }

    void joinAll()
    {
        for (size_t i=0; i<threads_.size(); ++i) {
            queue_.put("stop");
        }

        for_each(threads_.begin(), threads_.end(), 
                boost::bind(&muduo::Thread::join, _1));
    }

private:
    void threadFunc()
    {
        printf("tid=%d, %s started\n",
                muduo::CurrentThread::tid(),
                muduo::CurrentThread::name());

        latch_.countDown();

        bool running = true;
        while (running) {
            std::string d(queue_.take());
            printf("tid=%d, get data = %s, size = %zd\n", 
                    muduo::CurrentThread::tid(), d.c_str(), queue_.size());

            running = (d != "stop");
        }

        printf("tid=%d, %s stopped\n",
                muduo::CurrentThread::tid(),
                muduo::CurrentThread::name());
    }

private:
    muduo::CountDownLatch latch_;
    boost::ptr_vector<muduo::Thread> threads_;
    muduo::BoundBlockingQueue<std::string> queue_;
};

int main()
{
    printf("pid=%d, tid=%d\n", ::getpid(), muduo::CurrentThread::tid());

    Test t(5);
    t.run(100);
    t.joinAll();

    printf("number of created threads %d\n", muduo::Thread::numCreated());
}
