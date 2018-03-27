/*
 * Singleton.h
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

#ifndef MUDUO_BASE_SINGLETON_H
#define MUDUO_BASE_SINGLETON_H

#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <stdlib.h>

namespace muduo
{
template <typename T>
class Singleton : boost::noncopyable
{
public:
    static T& instance()
    {
        pthread_once(&ponce_, &Singleton::init);
        return *value_;
    }
private:
    Singleton();
    ~Singleton();

    static void init()
    {
        value_ = new T();
        ::atexit(destroy);
    }

    static void destroy()
    {
        // 1.complate type (编译时确定大小)和 incomplate type(编译大小为0)
        // 2.incomplate type 类型在delete的时候是有风险的，不会调用析构函数
        //  从而操作内存泄露
        // 3. 同时incomplate type在编译的时候大小0，数组的大小不允许为负数
        //  从而报错
        typedef char T_must_bo_complete_type[sizeof(T) == 0 ? -1 : 1];
        delete value_;
    }
private:
    static pthread_once_t   ponce_;
    static T *              value_;

};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T *Singleton<T>::value_ = NULL;

}

#endif // MUDUO_BASE_SINGLETON_H
