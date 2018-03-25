/*
 * Atmoic_unittest.cc
 * Copyright (c) 2018 liu <714049438@qq.com>
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

#include <muduo/base/Atomic.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    {
        muduo::AtomicInt64 a0;
        assert(a0.get() == 0);
        assert(a0.getAndAdd(1) == 0);
        assert(a0.get() == 1);
        assert(a0.addAndGet(2) == 3);
        assert(a0.get() == 3);
        assert(a0.incrementAndGet() == 4);
        assert(a0.get() == 4);
        a0.increment();
        assert(a0.get() == 5);
        assert(a0.addAndGet(-3) == 2);
        assert(a0.getAndSet(100) == 2);
        assert(a0.get() == 100);
    }

    {
        muduo::AtomicInt32 a1;
        assert(a1.get() == 0);
        assert(a1.getAndAdd(1) == 0);
        assert(a1.get() == 1);
        assert(a1.addAndGet(2) == 3);
        assert(a1.get() == 3);
        assert(a1.incrementAndGet() == 4);
        assert(a1.get() == 4);
        a1.increment();
        assert(a1.get() == 5);
        assert(a1.addAndGet(-3) == 2);
        assert(a1.getAndSet(100) == 2);
        assert(a1.get() == 100);
    }
    return 0;
}
