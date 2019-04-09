#ifndef STRING_PIECE_H__
#define STRING_PIECE_H__

#include <string.h>
#include <stddef.h>  // for ptrdiff_t
#include <assert.h>
#include <string>
#include <iosfwd>    // for ostream forward-declaration

#if 0
#define HAVE_TYPE_TRAITS
#include <type_traits.h>
#elif 0
#define HAVE_TYPE_TRAITS
#include <bits/type_traits.h>
#endif

//using std::memcmp;
//using std::strlen;
using std::string;

// see pcre_stringpiece.h and leveldb::slice.h
// StringPiece is a simple structure containing a pointer into some external
// storage and a size.  The user of a StringPiece must ensure that the StringPiece
// is not used after the corresponding external storage has been
// deallocated.
//
// Multiple threads can invoke const methods on a StringPiece without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same StringPiece must use
// external synchronization.

class StringPiece
{
private:
	const char*   ptr_;
	size_t        length_;

public:
	// standard STL container boilerplate
	typedef unsigned int uint32_t;
	typedef size_t size_type;
	typedef char value_type;
	typedef const char* pointer;
	typedef const char& reference;
	typedef const char& const_reference;
	typedef ptrdiff_t difference_type;
	typedef const char* const_iterator;
	typedef const char* iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;

	static const size_type npos = ~size_type(0);

public:
	// We provide non-explicit singleton constructors so users can pass
	// in a "const char*" or a "string" wherever a "StringPiece" is
	// expected.
	StringPiece()
		: ptr_(NULL), length_(0) { }
	StringPiece(const char* str)
		: ptr_(str), length_(strlen(ptr_)) { }
	StringPiece(const unsigned char* str)
		: ptr_(reinterpret_cast<const char*>(str)),
		length_(static_cast<int>(strlen(ptr_))) { }
	StringPiece(const string& str)
		: ptr_(str.data()), length_(str.size()) { }
	StringPiece(const char* offset, int len)
		: ptr_(offset), length_(len) { }

	// data() may return a pointer to a buffer with embedded NULs, and the
	// returned buffer may or may not be null terminated.  Therefore it is
	// typically a mistake to pass data() to a routine that expects a NUL
	// terminated string.  Use "as_string().c_str()" if you really need to do
	// this.  Or better yet, change your routine so it does not rely on NUL
	// termination.
	const char* data() const { return ptr_; }
	size_t size() const { return length_; }
	size_t length() const { return length_; }
	bool empty() const { return length_ == 0; }
	void clear() { ptr_ = NULL; length_ = 0; }
	iterator begin() const { return ptr_; }
	iterator end() const { return ptr_ + length_; }
	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(ptr_ + length_);
	}
	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(ptr_);
	}

	size_type max_size() const { return length_; }
	size_type capacity() const { return length_; }

	void set(const char* buffer, int len)
	{
		ptr_ = buffer;
		length_ = len;
	}
	void set(const char* str)
	{
		ptr_ = str;
		length_ = str ? strlen(str) : 0;
	}
	void set(const void* buffer, int len)
	{
		ptr_ = reinterpret_cast<const char*>(buffer);
		length_ = len;
	}

	char operator[](int i) const { return ptr_[i]; }

	void remove_prefix(int n)
	{
		ptr_ += n;
		length_ -= n;
	}

	void remove_suffix(int n)
	{
		length_ -= n;
	}

	bool operator==(const StringPiece& x) const
	{
		return ((length_ == x.length_) && (memcmp(ptr_, x.ptr_, length_) == 0));
	}
	bool operator!=(const StringPiece& x) const
	{
		return !(*this == x);
	}



	int compare(const StringPiece& x) const
	{
		int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
		if (r == 0)
		{
			if (length_ < x.length_) r = -1;
			else if (length_ > x.length_) r = +1;
		}
		return r;
	}
	int ignore_case_compare(const StringPiece& other) const;
	bool ignore_case_equal(const StringPiece& other) const;

	string as_string() const
	{
		return string(data(), size());
	}
	void copy_to_string(string* target) const
	{
		target->assign(ptr_, length_);
	}
	void append_to_string(std::string* target) const
	{
		if (!empty())
			target->append(data(), size());
	}

	// Does "this" start with "x"
	bool starts_with(const StringPiece& x) const
	{
		return ((length_ >= x.length_) && (memcmp(ptr_, x.ptr_, x.length_) == 0));
	}

	// Does "this" end with "x"
	bool ends_with(const StringPiece& x) const
	{
		return ((length_ >= x.length_) && (memcmp(ptr_, x.ptr_, x.length_) == 0));
	}

	size_type find(const StringPiece& s, size_type pos = 0) const;
	size_type find(char c, size_type pos = 0) const;
	size_type rfind(const StringPiece& s, size_type pos = npos) const;
	size_type rfind(char c, size_type pos = npos) const;

	size_type find_first_of(const StringPiece& s, size_type pos = 0) const;
	size_type find_first_of(char c, size_type pos = 0) const {
		return find(c, pos);
	}
	size_type find_first_not_of(const StringPiece& s, size_type pos = 0) const;
	size_type find_first_not_of(char c, size_type pos = 0) const;
	size_type find_last_of(const StringPiece& s, size_type pos = npos) const;
	size_type find_last_of(char c, size_type pos = npos) const {
		return rfind(c, pos);
	}
	size_type find_last_not_of(const StringPiece& s, size_type pos = npos) const;
	size_type find_last_not_of(char c, size_type pos = npos) const;

	StringPiece substr(size_type pos, size_type n = npos) const
	{
		if (pos > length_)
			pos = length_;
		if (n > length_ - pos)
			n = length_ - pos;
		return StringPiece(ptr_ + pos, int(n));
	}
};



// ------------------------------------------------------------------
// Functions used to create STL containers that use StringPiece
//  Remember that a StringPiece's lifetime had better be less than
//  that of the underlying string or char*.  If it is not, then you
//  cannot safely store a StringPiece into an STL container
// ------------------------------------------------------------------

#ifdef HAVE_TYPE_TRAITS
// This makes vector<StringPiece> really fast for some STL implementations
template<> struct __type_traits<StringPiece> {
	typedef __true_type    has_trivial_default_constructor;
	typedef __true_type    has_trivial_copy_constructor;
	typedef __true_type    has_trivial_assignment_operator;
	typedef __true_type    has_trivial_destructor;
	typedef __true_type    is_POD_type;
};
#endif

// allow StringPiece to be logged
std::ostream& operator<<(std::ostream& o, const StringPiece& piece);

#endif 
