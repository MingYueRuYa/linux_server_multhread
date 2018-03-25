/*
 * Types.h
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

#ifndef MUDUO_BASE_TYPES_H
#define MUDUO_BASE_TYPES_H

#include <stdint.h>
#ifdef MUDUO_STD_STRING
#include <string>
#else   // !MUDUO_STD_STRING
#include <ext/vstring.h>
#include <ext/vstring_fwd.h>
#endif //MUDUO_STD_STRING

namespace muduo
{
#ifdef MUDUO_STD_STRING
using std::string;
#else
typedef __gnu_cxx::__sso_string string;
#endif //MUDUO_STD_STRING

template<typename To, typename From>
inline To implicit_cast(From const & f)
{
    return f;
}

template<typename To, typename From>
inline To down_cast(From *f)
{
    // Ensures that To is a sub-type of From *.  This test is here only
    // for compile-time type checking, and has no overhead in an
    // optimized build at run-time, as it will be optimized away
    // completely.
    if (false) {
        implicit_cast<From *, To>(0);
    }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
    assert(f == NULL || dynamic_cast<To>(f) != NULL);
#endif 
    //TODO: 这里的返回类型To对？好像是To*才对
    return static_cast<To>(f);
}

}

#endif // MUDUO_BASE_TYPES_H
