/*
 * Atomic.h
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

#ifndef MUDUO_BASE_ATOMIC_H
#define MUDUO_BASE_ATOMIC_H

#include <boost/noncopyable.hpp>
#include <stdint.h>

namespace muduo
{

namespace detail
{
template<typename T>
class AtomicIntegerT: boost::noncopyable
{
public:
    AtomicIntegerT() : value_(0)
    {}

    // uncomment if you need copying and assignment
    // AtomicIntegerT(const AtomicIntegerT& that)
    //   : value_(that.get())
    // {}
    //
    // AtomicIntegerT& operator=(const AtomicIntegerT& that)
    // {
    //   getAndSet(that.get());
    //   return *this;
    // }

    T get()
    { return __sync_val_compare_and_swap(&value_, 0, 0); }

    T getAndAdd(T x)
    { return __sync_fetch_and_add(&value_, x); }

    T addAndGet(T x)
    { return getAndAdd(x) + x; }

    T incrementAndGet()
    { return addAndGet(1); }

    T decrementAndGet()
    { return addAndGet(-1); }

    void add(T x)
    { getAndAdd(x); }

    void increment()
    { incrementAndGet(); }

    void decrement()
    { decrementAndGet(); }

    T getAndSet(T newValue)
    { return __sync_lock_test_and_set(&value_, newValue); }


private:
    volatile T value_;

};
}

typedef detail::AtomicIntegerT<int32_t> AtomicInt32;
typedef detail::AtomicIntegerT<int64_t> AtomicInt64;

}

#endif // MUDUO_BASE_ATOMIC_H
