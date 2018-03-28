/*
 * ThreadLocalSingleton.h
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

#ifndef MUDUO_BASE_THREADLOCALSINGLETON_H
#define MUDUO_BASE_THREADLOCALSINGLETON_H

#include <boost/noncopyable.hpp>
#include <assert.h>
#include <pthread.h>

namespace muduo
{
template <typename T>
class ThreadLocalSingleton : boost::noncopyable
{
public:
    static T &instance()
    {
        if (NULL == t_value_) {
            t_value_ = new T();
            deleter_.set(t_value_);
        }
        return *t_value_;
    }

    static T *pointer()
    { return t_value_; }

private:
    static void destructor(void *obj)
    {
        assert(obj == t_value_);
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
        delete t_value_;
        t_value_ = 0;
    }

private:
    class Deleter
    {
        public:
            Deleter()
            { pthread_key_create(&pkey_, &ThreadLocalSingleton::destructor); }

            ~Deleter()
            { pthread_key_delete(pkey_); }

            void set(T *newObj)
            {
                assert(pthread_getspecific(pkey_) == NULL);
                pthread_setspecific(pkey_, newObj);
            }

        private:
            pthread_key_t pkey_;
    };

private:
    static __thread T *t_value_;
    static Deleter  deleter_;
};

template<typename T>
__thread T*ThreadLocalSingleton<T>::t_value_ = NULL;

template<typename T>
typename ThreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;

}

#endif // MUDUO_BASE_THREADLOCALSINGLETON_H
