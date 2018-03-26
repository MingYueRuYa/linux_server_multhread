/*
 * CountDownLatch1.cc
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

#include <muduo/base/CountDownLatch.h>
#include <muduo/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>
#include <stdio.h>

/*
 * 此模型：是主线程initializer finished, 子线程再启动
 * */
using namespace muduo;

class Test
{
public:
    Test(int numThreads)
        : latch_(1),
        threads_(numThreads)
    {
        for (int i=0; i<numThreads; ++i) {
            char name[32];
            snprintf(name, sizeof name, "work thread %d", i);
            threads_.push_back(
                    new muduo::Thread(boost::bind(&Test::threadFunc, this), 
                        muduo::string(name)));
        }
        for_each(threads_.begin(), threads_.end(), 
            boost::bind(&Thread::start, _1)); 
    }

    void run()
    { latch_.countDown(); }

    void joinAll()
    { 
        for_each(threads_.begin(), threads_.end(), 
            boost::bind(&Thread::join, _1)); 
    }

private:
    void threadFunc()
    {
        latch_.wait();
        printf("tid=%d, %s started\n",
                CurrentThread::tid(), CurrentThread::name());

        printf("tid=%d, %s stopped\n",
                CurrentThread::tid(), CurrentThread::name());
    }

private:
    CountDownLatch latch_;
    boost::ptr_vector<Thread> threads_;
};

int main(void)
{
    printf("pid=%d, tid=%d\n", ::getpid(), CurrentThread::tid());

    Test t(3);

    sleep(3);

    printf("pid=%d, tid=%d, name=%s, runing ...\n", 
            ::getpid(), CurrentThread::tid(), CurrentThread::name());

    t.run();
    t.joinAll();

    printf("number of created threads %d\n", Thread::numCreated());

    return 0;
}
