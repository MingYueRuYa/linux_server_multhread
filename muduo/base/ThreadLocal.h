/*
 * ThreadLocal.h
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

#ifndef MUDUO_BASE_THREADLOCAL_H
#define MUDUO_BASE_THREADLOCAL_H

#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace muduo
{
template <typename T>
class ThreadLocal : boost::noncopyable
{
public:
    ThreadLocal()
    { pthread_key_create(&pkey_, &ThreadLocal::destructor); }

    ~ThreadLocal()
    { pthread_key_delete(pkey_); }

    T &value()
    {
        T *perThreadValue = static_cast<T *>(pthread_getspecific(pkey_));
        if (! perThreadValue) {
            T *newObj = new T();
            pthread_setspecific(pkey_, newObj);
            perThreadValue = newObj;
        }
        return *perThreadValue;
    }

private:
    static void destructor(void *x)
    {
        T *obj = static_cast<T *>(x);
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
        delete obj;
    }

private:
    pthread_key_t pkey_;
};
}

#endif // MUDUO_BASE_THREADLOCAL_H
