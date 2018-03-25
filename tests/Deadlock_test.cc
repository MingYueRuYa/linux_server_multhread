/*
 * Deadlock_test.cc
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

// 一个在多线程程序里fork造成死锁的例子
// // 一个输出示例：
// /*
//
// pid = 19445 Entering main ...
// pid = 19445 begin doit ...
// pid = 19447 begin doit ...
// pid = 19445 end doit ...
// pid = 19445 Exiting main ...
//
// 父进程在创建了一个线程，并对mutex加锁，
// 父进程创建一个子进程，在子进程中调用doit，由于子进程会复制父进程的内存
// ，这时候mutex处于锁的状态，
// 父进程在复制子进程的时候，只会复制当前线程的执行状态，其它线程不会复制
// 。因此子进程会处于死锁的状态。

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *doit(void *arg)
{
    printf("pid = %d begin doit ...\n", static_cast<int>(getpid()));
    pthread_mutex_lock(&mutex);
    struct timespec ts = {2, 0};
    nanosleep(&ts, NULL);
    pthread_mutex_unlock(&mutex);
    printf("pid = %d end doit ...\n", static_cast<int>(getpid()));
    return NULL;
}

int main()
{
    printf("pid = %d Entering main ...\n", static_cast<int>(getpid()));
    pthread_t tid;
    pthread_create(&tid, NULL, doit, NULL);
    struct timespec ts = {1, 0};
    nanosleep(&ts, NULL);

    if (0 == fork()) {  // 子进程
        doit(NULL);
    }
    pthread_join(tid, NULL);

    printf("pid = %d Exiting main ...\n", static_cast<int>(getpid()));
    return 0;
}
