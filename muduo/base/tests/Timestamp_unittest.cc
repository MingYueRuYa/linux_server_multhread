/*
 * Timestamp_unittest.cc
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



#include <muduo/base/Timestamp.h>
#include <vector>
#include <stdio.h>

using muduo::Timestamp;

void passByConstReference(const Timestamp& x)
{
    printf("%s\n", x.toString().c_str());
}

void passByValue(Timestamp x)
{
    printf("%s\n", x.toString().c_str());
}

void benckmark()
{
    const int kNumber = 1000*1000;
    std::vector<Timestamp> stamps;
    stamps.reserve(kNumber);
    for (int i = 0; i<kNumber; ++i) {
        stamps.push_back(Timestamp::now());
    }

    printf("%s\n", stamps.front().toString().c_str());
    printf("%s\n", stamps.back().toString().c_str());
    printf("%f\n", timeDifference(stamps.back(), stamps.front()));

    int increments[100] = {0};
    int64_t start = stamps.front().microSecondsSinceEpoch();
    for (int i=1; i<kNumber; ++i) {
        int64_t next = stamps[i].microSecondsSinceEpoch();
        int64_t inc = next - start;
        start = next;
        if (inc < 0) {
            printf("reverse\n");
        } else if (inc < 100) {
            ++increments[inc];
        } else {
            printf("big gap %d\n", static_cast<int>(inc));
        }
    }

    for (int i=0; i<100; ++i) {
        printf("%2d: %d\n", i, increments[i]);
    }

}

int main(int argc, char *argv[])
{
    Timestamp now(Timestamp::now());
    
    printf("%s\n", now.toString().c_str());

    passByValue(now);

    passByConstReference(now);

    benckmark();

    return 0;
}
















