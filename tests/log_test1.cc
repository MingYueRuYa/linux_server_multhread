/*
 * log_test1.cc
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

#include <muduo/base/Logging.h>
#include <errno.h>
#include <stdio.h>

using namespace muduo;

FILE *g_file;

void dummyOutput(const char *msg, int len)
{
    if (g_file) {
        fwrite(msg, 1, len, g_file);
    }
}

void dummyFinish()
{
    fflush(g_file);
}

int main()
{
    g_file = ::fopen("/tmp/muduo_log", "ae");
    Logger::setOutput(dummyOutput);
    Logger::setFlush(dummyFinish);

    LOG_TRACE << "trace...";
    LOG_DEBUG << "debug...";
    LOG_INFO  << "info...";
    LOG_WARN  << "warn...";
    LOG_ERROR << "error...";

    errno = 13;
    LOG_SYSERR << "syserr ...";

    ::fclose(g_file);
    return 0;
}

