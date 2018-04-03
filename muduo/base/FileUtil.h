/*
 * FileUtil.h
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

#ifndef MUDUO_BASE_FILEUTIL_H
#define MUDUO_BASE_FILEUTIL_H

#include <muduo/base/Types.h>
#include <muduo/base/StringPiece.h>
#include <boost/noncopyable.hpp>

namespace muduo
{

namespace FileUtil
{
    class SmallFile : boost::noncopyable
    {
    public:
        SmallFile(StringPiece filename);
        ~SmallFile();

        //return errno
        template<typename String>
        int readToString(int maxSize, 
                        String *content, 
                        int64_t *fileSize,
                        int64_t *modifyTime,
                        int64_t *createTime);

        // return errno
        int readToBuffer(int *size);
        const char *buffer() const { return buf_; }
        static const int kBufferSize = 65536;

    private:
        int fd_;
        int err_;
        char buf_[kBufferSize];
    }; 

    // read the file content, returns errno if error happens
    template <typename String>
    int readFile(StringPiece filename, int maxSize, String *content = NULL,
            int64_t *fileSize = NULL,int64_t *modifyTime = NULL,
            int64_t *createTime = NULL) 
    {
        SmallFile file(filename);
        return file.readToString(maxSize, content, fileSize, 
                                 modifyTime, createTime);
    }
} // FileUtil

} // muduo

#endif // MUDUO_BASE_FILEUTIL_H
