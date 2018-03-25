/*
 * Condition.h
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

#ifndef MUDUO_BASE_CONDITION_H
#define MUDUO_BASE_CONDITION_H

#include <muduo/base/Mutex.h>

#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace muduo
{
class Condition : boost::noncopyable
{
public:
    explicit Condition(MutexLock &mutex)
        : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }

    ~Condition() { pthread_cond_destroy(&pcond_); }

    void wait() { pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()); }

    // return true if time out, false otherwise
    bool waitForSeconds(int seconds);

    void notify() { pthread_cond_signal(&pcond_); }
    void notifyAll() { pthread_cond_broadcast(&pcond_); }

private:
    MutexLock &mutex_;
    pthread_cond_t pcond_;
};
}

#endif //MUDUO_BASE_CONDITION_H
