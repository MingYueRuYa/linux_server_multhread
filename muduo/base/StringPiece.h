/*
 * StringPiece.h
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

#ifndef MUDUO_BASE_STRINGPIECE_H
#define MUDUO_BASE_STRINGPIECE_H

#include <string.h>
#include <iosfwd>

#include <muduo/base/Types.h>

#ifndef MUDUO_STD_STRING
#include <string>
#endif // MUDUO_STD_STRING

namespace muduo
{
class StringPiece
{
private:
    const char  *ptr_;
    int         length_;

public:
    StringPiece()
        : ptr_(NULL), length_(0)
    {}

    StringPiece(const char *str)
        : ptr_(str), length_(static_cast<int>(strlen(ptr_)))
    {}

    StringPiece(const unsigned char *str)
        : ptr_(reinterpret_cast<const char *>(str)),
          length_(static_cast<int>(strlen(ptr_)))
    {}

    StringPiece(const string &str)
        : ptr_(str.data()),
          length_(static_cast<int>(str.size()))
    {}

#ifndef MUDUO_STD_STRING    
    StringPiece(const std::string &str)
        : ptr_(str.data()),
          length_(static_cast<int>(str.size()))
    {}
#endif  // MUDUO_STD_STRING

    StringPiece(const char *offset, int len)
        : ptr_(offset),
          length_(len)
    {}

    const char *data() const { return ptr_; }
    int size() const { return length_; }
    bool empty() const { return length_ == 0; }

    void clear() 
    { 
        ptr_    = NULL;
        length_ = 0; 
    }

    void set(const char *buffer, int len)
    { 
        ptr_    = buffer;
        length_ = len; 
    }

    void set(const char *str)
    {
        ptr_    = str;
        length_ = static_cast<int>(strlen(str));
    }

    void set(const void *buffer, int len)
    {
        ptr_    = reinterpret_cast<const char *>(buffer);
        length_ = len;
    }

    char operator[] (int i) const { return ptr_[i]; }

    void remove_prefix(int n)
    {
        ptr_    += n;
        length_ -= n;
    }

    void remove_suffix(int n)
    {
        length_ -= n;
    }

    bool operator==(const StringPiece & x) const
    {
        return ((length_ == x.length_) && 
            (0 == memcmp(ptr_, x.ptr_, length_)));
    }

    bool operator!= (const StringPiece &x) const
    { return !(*this == x); }

#define STRINGPIECE_BINARY_PREDICATE(cmp, auxcmp)   \
bool operator cmp(const StringPiece &x) const {  \
int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_); \
    return ((r auxcmp 0) || ((r == 0) && (length_ cmp x.length_))); \
}

STRINGPIECE_BINARY_PREDICATE(<, <);
STRINGPIECE_BINARY_PREDICATE(<=, <);
STRINGPIECE_BINARY_PREDICATE(>=, >);
STRINGPIECE_BINARY_PREDICATE(>, >);

#undef STRINGPIECE_BINARY_PREDICATE

    int compare(const StringPiece &x) const
    {
        int r = memcmp(ptr_, x.ptr_, 
                length_ < x.length_ ? length_ : x.length_);
        
        if (0 == r) {
            if (length_ < x.length_) {
                r = -1;
            } else if (length_ > x.length_) {
                r = +1;
            }
        }
        return r;
    }

    string as_string() const 
    {
        return string(data(), size());
    }

    void CopyToString(string *target) const
    {
        target->assign(ptr_, length_);
    }

    #ifndef MUDUO_STD_STRING
    void CopyToStdString(std::string *target) const
    {
        target->assign(ptr_, length_);
    }
    #endif // MUDUO_STD_STRING

    bool starts_with(const StringPiece &x) const
    {
        return ((length_ >= x.length_) && 
                 (0 == memcmp(ptr_, x.ptr_, x.length_)));
    }
};
}   // namespace muduo

#ifdef HAVE_TYPE_TRAITS
template<> struct __type_traits<muduo::StringPiece>
{
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};
#endif // HAVE_TYPE_TRAITS

// allow StringPiece to be logged
std::ostream &operator<<(std::ostream &o, const muduo::StringPiece &piece);

#endif // MUDUO_BASE_STRINGPIECE_H
