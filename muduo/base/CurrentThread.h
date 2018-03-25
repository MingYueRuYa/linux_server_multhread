/*
 * CurrentThread.h
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

#ifndef MUDUO_BASE_CURRENTTHREAD_H
#define MUDUO_BASE_CURRENTTHREAD_H

namespace muduo
{
namespace CurrentThread
{
    // internal
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread const char *t_threadName;

    void cacheTid();
    
    inline int tid()
    {
        if (0 == t_cachedTid) {
            cacheTid();
        }
        return t_cachedTid;
    }

    inline const char *tidString()
    {
        return t_tidString;
    }

    inline const char *name()
    {
        return t_threadName;
    }

    bool isMainThread();
}
}

#endif // MUDUO_BASE_CURRENTTHREAD_H

