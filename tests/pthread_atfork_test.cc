/*
 * pthread_atfork_test.cc
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

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

void prepare(void)
{
    printf("pid = %d prepare...\n", static_cast<int>(getpid()));
}

void parent(void)
{
    printf("pid = %d parent...\n", static_cast<int>(getpid()));
}

void child(void)
{
    printf("pid = %d child...\n", static_cast<int>(getpid()));
}

int main()
{
    printf("pid = %d Entering main ...\n", static_cast<int>(getpid()));

    pthread_atfork(prepare, parent, child);

    fork();

    printf("pid = %d Exiting main...\n", static_cast<int>(getpid()));

    return 0;
}
