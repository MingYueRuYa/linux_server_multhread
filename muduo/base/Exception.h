/*
 * Exception.h
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

#ifndef MUDUO_BASE_EXCEPTION_H
#define MUDUO_BASE_EXCEPTION_H

#include <muduo/base/Types.h>
#include <exception>

namespace muduo
{
class Exception : public std::exception
{
public:    
    explicit Exception(const char *what);
    explicit Exception(const string &wath);
    virtual ~Exception() throw();
    
    virtual const char *what() const throw();
    const char *stackTrace() const throw();
    
private:
    void fillStackTrace();
    string demangle(const char *symbol);

private:
    string message_;
    string stack_;
};
}

#endif //MUDUO_BASE_EXCEPTION_H
