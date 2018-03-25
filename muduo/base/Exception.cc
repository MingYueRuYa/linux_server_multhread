/*
 * Exception.cc
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

#include <muduo/base/Exception.h>
#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>

using namespace muduo;

Exception::Exception(const char *msg)
    : message_(msg)
{
    fillStackTrace();
}

Exception::Exception(const string &msg)
    : message_(msg)
{
    fillStackTrace();
}

Exception::~Exception() throw()
{}

const char *Exception::what() const throw()
{
    return message_.c_str();
}

const char *Exception::stackTrace() const throw()
{
    return stack_.c_str();
}

void Exception::fillStackTrace()
{
    const int len = 200;
    void *buffer[len];

    int nptrs       = ::backtrace(buffer, len);
    char **strings  = ::backtrace_symbols(buffer, nptrs);
    if (NULL == strings) {
        return;
    }

    for (int i=0; i<nptrs; ++i) {
        stack_.append(demangle(strings[i]));
        stack_.push_back('\n');
    }

    free(strings);
}

string Exception::demangle(const char *symbol)
{
    size_t size     = 0;
    int status      = -1;
    char temp[128]  = {0};
    char *demangled = NULL;

    //first ,try to demangle a c++ name
    if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp)) {
        if (NULL != (demangled = abi::__cxa_demangle(temp, 
                        NULL, &size, &status))) {
            string result(demangled);
            free(demangled);
            return result;
        }
    }

    //if that didn't work, try to get a regular c symbol 
    if (1 == sscanf(symbol, "%127s", temp)) {
        return temp;
    }

    // if all else fials, just return the symbol
    return symbol;
}
