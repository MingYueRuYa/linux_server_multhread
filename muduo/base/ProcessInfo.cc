/*
 * ProcessInfo.cc
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

#include <muduo/base/ProcessInfo.h>
#include <muduo/base/FileUtil.h>

#include <algorithm>
#include <assert.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

namespace muduo
{
namespace detail
{
    __thread int t_numOpendedFiles = 0;

    int fdDirFilter(const struct dirent *d)
    {
        if (::isdigit(d->d_name[0])) {
            ++t_numOpendedFiles;
        }
        return 0;
    }

    __thread std::vector<pid_t> *t_pids = NULL;
    int taskDirFilter(const struct dirent *d)
    {
        if (::isdigit(d->d_name[0])) {
            t_pids->push_back(atoi(d->d_name));
        }
        return 0;
    }

    int scanDir(const char *dirpath, int(*filter)(const struct dirent *))
    {
        struct dirent **namelist = NULL;
        int result = ::scandir(dirpath, &namelist, filter, alphasort);
        assert(namelist == NULL);
        return result;
    }

    Timestamp g_startTime = Timestamp::now();

} // detail
} // muduo

using namespace muduo;
using namespace muduo::detail;

pid_t ProcessInfo::pid()
{
    return ::getpid();
}

string ProcessInfo::pidString()
{
    char buf[32];
    snprintf(buf, sizeof buf, "%d", pid());
}

uid_t ProcessInfo::uid()
{
    return ::getuid();
}

string ProcessInfo::username()
{
    struct passwd pwd;
    struct passwd *result = NULL;
    char buf[8192];
    const char *name = "unknownuser";

    getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
    if (result) {
        name = pwd.pw_name;
    }
    return name;
}

uid_t ProcessInfo::euid()
{
    return ::geteuid();
}

Timestamp ProcessInfo::startTime()
{
    return g_startTime;
}

string ProcessInfo::hostname()
{
    char buf[64] = "unknowhost";
    buf[sizeof(buf)-1] = '\0';
    ::gethostname(buf, sizeof buf);
    return buf;
}

// read /proc/self/status
string ProcessInfo::procStatus()
{
    string result;
    FileUtil::readFile("/proc/self/status", 65536, &result);
    return result;
}

int ProcessInfo::openedFiles()
{
    t_numOpendedFiles = 0;
    scanDir("/proc/self/fd", fdDirFilter);
    return t_numOpendedFiles;
}

int ProcessInfo::maxOpenFiles()
{
    struct rlimit rl;
    if (::getrlimit(RLIMIT_NOFILE, &rl)) {
        return openedFiles(); 
    } else {
        return static_cast<int>(rl.rlim_cur);
    }
}

int ProcessInfo::numThreads()
{
    int result = 0;
    string status = procStatus();
    size_t pos = status.find("Threads:");
    if (pos == string::npos) {
        return result;
    }

    result = ::atoi(status.c_str() + pos + 8);
    return result;
}

std::vector<pid_t> ProcessInfo::threads()
{
    std::vector<pid_t> result;
    t_pids = &result;

    scanDir("/proc/self/task", taskDirFilter);
    t_pids = NULL;
    std::sort(result.begin(), result.end());
    return result;
}
