#pragma once

#include <stdint.h>     // for standard types
#include <string.h>     // for memcpy, etc
#include <algorithm>    // for std::swap
#include "Log.h"


namespace Luft
{
	class lstrliteral
	{
		const char* str;
		size_t len;

		// make the literal operator a friend so it can construct fixed strings. No-one else can.
		friend lstrliteral operator"" _lit(const char* str, size_t len);

		// similarly friend inflexible strings to allow them to decompose to a literal
		friend class rdcinflexiblestr;

		lstrliteral(const char* s, size_t l) : str(s), len(l) {}
		lstrliteral() = delete;

	public:
		const char* c_str() const { return str; }
		size_t length() const { return len; }
		const char* begin() const { return str; }
		const char* end() const { return str + len; }
	};

	inline lstrliteral operator"" _lit(const char* str, size_t len)
	{
		return lstrliteral(str, len);
	}

	class lstr
	{
	private:
		// ARRAY_STATE is deliberately 0 so that 0-initialisation is a valid empty array string
		static constexpr size_t ARRAY_STATE = size_t(0);
		static constexpr size_t ALLOC_STATE = size_t(1) << ((sizeof(size_t) * 8) - 2);
		static constexpr size_t FIXED_STATE = size_t(1) << ((sizeof(size_t) * 8) - 1);

		struct alloc_ptr_rep
		{
			// we reserve two bits but we only have three states
			static constexpr size_t CAPACITY_MASK = ~size_t(0) >> 2;
			static constexpr size_t STATE_MASK = ~CAPACITY_MASK;

			// storage
			char* str;
			// the current size of the string (less than or equal to capacity). Doesn't include NULL
			size_t size;

			size_t get_capacity() const { return capacity & CAPACITY_MASK; }
			void set_capacity(size_t c) { capacity = ALLOC_STATE | c; }
		private:
			size_t capacity;
		};

		struct fixed_ptr_rep
		{
			const char* str;
			size_t size;
			size_t flags;
		};

		struct arr_rep
		{
			// all bytes except the last one are used for storing short strings
			char str[sizeof(size_t) * 3 - 1];
			// capacity is fixed - 1 less than the number of characters above (so we always have room for the NULL terminator)
			static const size_t capacity = sizeof(arr_rep::str) - 1;

			size_t get_size() const { return _size; }
			void set_size(size_t s) { _size = (unsigned char)s; }
		private:
			// we only have 6-bits of this available is enough for up to 63 size, more than what we can
			// store anyway.
			unsigned char _size;
		};

		// zero-initialised this becomes an empty string in array format
		union string_data
		{
			// stored as size, capacity, and pointer to d
			alloc_ptr_rep alloc;
			// stored as size and pointer
			fixed_ptr_rep fixed;
			// stored as in-line array
			arr_rep arr;
		} d;

		bool is_alloc() const { return !!(d.fixed.flags & ALLOC_STATE); }
		bool is_fixed() const { return !!(d.fixed.flags & FIXED_STATE); }
		bool is_array() const { return !is_alloc() && !is_fixed(); }

		static char* allocate(size_t count)
		{
			char* ret = NULL;
			ret = (char*)malloc(count);
			if (ret == NULL)
			{
				CORE_LOG_ERROR("Out of memory");
			}
			return ret;
		}

		static void deallocate(char* p)
		{
			free((void*)p);
		}

		// if we're not already mutable (i.e. fixed string) then change to a mutable string
		void ensure_mutable(size_t s = 0)
		{
			if (!is_fixed())
				return;

			const char* fixed_str = d.fixed.str;
			size_t fixed_size = d.fixed.size;
			// allocate at least enough for the string - reserve is non-destructive.
			if (s < fixed_size)
				s = fixed_size;
			// if we can satisfy the request with the array representation, it's easier
			if (s <= d.arr.capacity)
			{
				// copy d, we can safely include the NULL terminator we know is present
				memcpy(d.arr.str, fixed_str, fixed_size + 1);

				// store metadata
				d.arr.set_size(fixed_size);
			}
			else
			{
				// otherwise we need to allocate

				// allocate the requested size now, +1 for NULL terminator
				d.alloc.str = allocate(s + 1);
				// copy d, we can safely include the NULL terminator we know is present
				memcpy(d.alloc.str, fixed_str, fixed_size + 1);

				// store metadata
				d.alloc.set_capacity(fixed_size);
				d.alloc.size = fixed_size;
			}
		}
	public:
		lstr() { memset(&d, 0, sizeof(d)); }
		~lstr()
		{
			if (is_alloc())
				deallocate(d.alloc.str);
		}

		lstr(lstr&& in)
		{
			d = in.d;
			// the input no longer owns d. Set to 0 to be extra-clear
			memset(&in.d, 0, sizeof(d));
		}

		lstr& operator=(lstr&& in)
		{
			if (is_alloc())
				deallocate(d.alloc.str);

			d = in.d;
			memset(&in.d, 0, sizeof(d));
			return *this;
		}

		// special constructor from literals
		lstr(const lstrliteral& lit)
		{
			d.fixed.str = lit.c_str();
			d.fixed.size = lit.length();
			d.fixed.flags = FIXED_STATE;
		}

		// copy constructors forward to assign
		lstr(const lstr& in)
		{
			memset(&d, 0, sizeof(d));
			assign(in);
		}

		lstr(const char* const in)
		{
			memset(&d, 0, sizeof(d));
			assign(in, strlen(in));
		}
		lstr(const char* const in, size_t length)
		{
			memset(&d, 0, sizeof(d));
			assign(in, length);
		}
		// also operator=
		lstr& operator=(const lstr& in)
		{
			assign(in);
			return *this;
		}
		lstr& operator=(const char* const in)
		{
			assign(in, strlen(in));
			return *this;
		}

		inline void swap(lstr& other)
		{
			// just need to swap the d element
			std::swap(d, other.d);
		}

		// assign from an rdcstr, copy the d element and allocate if needed
		void assign(const lstr& in)
		{
			// do nothing if we're self-assigning
			if (this == &in)
				return;

			// if the input d is allocated, we need to make our own allocation. Go through the standard
			// string assignment function which will allocate & copy
			if (in.is_alloc())
			{
				assign(in.d.alloc.str, in.d.alloc.size);
			}
			else
			{
				// otherwise just deallocate if necessary and copy
				if (is_alloc())
					deallocate(d.alloc.str);

				d = in.d;
			}
		}

		// assign from something else
		void assign(const char* const in, size_t length)
		{
			// ensure we have enough capacity allocated
			reserve(length);

			// write to the string we're using, depending on if we allocated or not
			char* str = is_alloc() ? d.alloc.str : d.arr.str;
			// copy the string itself
			memcpy(str, in, length);
			// cap off with NULL terminator
			str[length] = 0;

			if (is_alloc())
				d.alloc.size = length;
			else
				d.arr.set_size(length);
		}
		void assign(const char* const str) { assign(str, strlen(str)); }

		// in-place modification functions
		void append(const char* const str) { append(str, strlen(str)); }
		void append(const lstr& str) { append(str.c_str(), str.size()); }
		void append(const char* const str, size_t length) { insert(size(), str, length); }
		void erase(size_t offs, size_t count)
		{
			const size_t sz = size();

			// invalid offset
			if (offs >= sz)
				return;

			if (count > sz - offs)
				count = sz - offs;

			char* str = data();
			for (size_t i = offs; i < sz - count; i++)
				str[i] = str[i + count];

			resize(sz - count);
		}

		void insert(size_t offset, const char* const str) { insert(offset, str, strlen(str)); }
		void insert(size_t offset, const lstr& str) { insert(offset, str.c_str(), str.size()); }
		void insert(size_t offset, char c) { insert(offset, &c, 1); }
		void insert(size_t offset, const char* const instr, size_t length)
		{
			if (!is_fixed() && instr + length >= begin() && end() >= instr)
			{
				lstr copy;
				copy.swap(*this);
				this->reserve(copy.capacity() + length);
				*this = copy;
				insert(offset, copy.c_str(), copy.size());
				return;
			}

			const size_t sz = size();

			// invalid offset
			if (offset > sz)
				return;

			// allocate needed size
			reserve(sz + length);

			// move anything after the offset upwards, including the NULL terminator by starting at sz + 1
			char* str = data();
			for (size_t i = sz + 1; i > offset; i--)
				str[i + length - 1] = str[i - 1];

			// copy the string to the offset
			memcpy(str + offset, instr, length);

			// increase the length
			if (is_alloc())
				d.alloc.size += length;
			else
				d.arr.set_size(sz + length);
		}

		void replace(size_t offset, size_t length, const lstr& str)
		{
			erase(offset, length);
			insert(offset, str);
		}

		// fill the string with 'count' copies of 'c'
		void fill(size_t count, char c)
		{
			resize(count);
			memset(data(), c, count);
		}

		// read-only by-value accessor can look up directly in c_str() since it can't be modified
		const char& operator[](size_t i) const { return c_str()[i]; }
		// assignment operator must make the string mutable first
		char& operator[](size_t i)
		{
			ensure_mutable();
			return is_alloc() ? d.alloc.str[i] : d.arr.str[i];
		}

		// stl type interface
		void reserve(size_t s)
		{
			if (is_fixed())
			{
				ensure_mutable(s);
				return;
			}

			const size_t old_capacity = capacity();

			// nothing to do if we already have this much space. We only size up
			if (s <= old_capacity)
				return;

			// if we're currently using the array representation, the current capacity is always maxed out,
			// meaning if we don't have enough space we *must* now allocate.

			const size_t old_size = is_alloc() ? d.alloc.size : d.arr.get_size();
			const char* old_str = is_alloc() ? d.alloc.str : d.arr.str;

			// either double, or allocate what's needed, whichever is bigger. ie. by default we double in
			// size but we don't grow exponentially in 2^n to cover a single really large resize
			if (old_capacity * 2 > s)
				s = old_capacity * 2;

			// allocate +1 for the NULL terminator
			char* new_str = allocate(s + 1);

			// copy the current characters over, including NULL terminator
			memcpy(new_str, old_str, old_size + 1);

			// deallocate the old storage
			if (is_alloc())
				deallocate(d.alloc.str);

			// we are now an allocated string
			d.alloc.str = new_str;

			// updated capacity
			d.alloc.set_capacity(s);
			// size is unchanged
			d.alloc.size = old_size;
		}

		void push_back(char c)
		{
			// store old size
			size_t s = size();

			// reserve enough memory and ensure we're mutable
			reserve(s + 1);

			// append the character
			if (is_alloc())
			{
				d.alloc.size++;
				d.alloc.str[s] = c;
				d.alloc.str[s + 1] = 0;
			}
			else
			{
				d.arr.set_size(s + 1);
				d.arr.str[s] = c;
				d.arr.str[s + 1] = 0;
			}
		}

		void pop_back()
		{
			if (!empty())
				resize(size() - 1);
		}

		void resize(const size_t s)
		{
			// if s is 0, fast path - if we're allocated just change the size, otherwise reset to an empty
			// array representation.
			if (s == 0)
			{
				if (is_alloc())
				{
					d.alloc.size = 0;
					d.alloc.str[0] = 0;
					return;
				}
				else
				{
					// either we're a fixed string, and we need to become an empty array, or we're already an
					// array in which case we empty the array.
					memset(&d, 0, sizeof(d));
					return;
				}
			}

			const size_t oldSize = size();

			// call reserve first. This handles resizing up, and also making the string mutable if necessary
			reserve(s);

			// if the size didn't change, return.
			if (s == oldSize)
				return;

			// now resize the string
			if (is_alloc())
			{
				// if we resized upwards, memset the new elements to 0, if we resized down set the new NULL
				// terminator
				if (s > oldSize)
					memset(d.alloc.str + oldSize, 0, s - oldSize + 1);
				else
					d.alloc.str[s] = 0;

				// update the size.
				d.alloc.size = s;
			}
			else
			{
				// if we resized upwards, memset the new elements to 0, if we resized down set the new NULL
				// terminator
				if (s > oldSize)
					memset(d.arr.str + oldSize, 0, s - oldSize + 1);
				else
					d.arr.str[s] = 0;

				// update the size.
				d.arr.set_size(s);
			}
		}

		size_t capacity() const
		{
			if (is_alloc())
				return d.alloc.get_capacity();
			if (is_fixed())
				return d.fixed.size;
			return d.arr.capacity;
		}
		size_t size() const
		{
			if (is_alloc() || is_fixed())
				return d.fixed.size;
			return d.arr.get_size();
		}
		size_t length() const { return size(); }

		const char* c_str() const
		{
			if (is_alloc() || is_fixed())
				return d.alloc.str;
			return d.arr.str;
		}

		void clear() { resize(0); }
		bool empty() const { return size() == 0; }
		const char* data() const { return c_str(); }
		char* data()
		{
			ensure_mutable();
			return is_alloc() ? d.alloc.str : d.arr.str;
		}
		const char* begin() const { return c_str(); }
		const char* end() const { return c_str() + size(); }
		char* begin() { return data(); }
		char* end() { return data() + size(); }
		char front() const { return *c_str(); }
		char& front()
		{
			ensure_mutable();
			return *data();
		}
		char back() const { return *(end() - 1); }
		char& back()
		{
			ensure_mutable();
			return data()[size() - 1];
		}

		lstr substr(size_t offs, size_t length = ~0U) const
		{
			const size_t sz = size();
			if (offs >= sz)
				return lstr();

			if (length == ~0U || offs + length > sz)
				length = sz - offs;

			return lstr(c_str() + offs, length);
		}

		lstr& operator+=(const char* const str)
		{
			append(str, strlen(str));
			return *this;
		}
		lstr& operator+=(const lstr& str)
		{
			append(str.c_str(), str.size());
			return *this;
		}
		lstr& operator+=(char c)
		{
			push_back(c);
			return *this;
		}
		lstr operator+(const char* const str) const
		{
			lstr ret = *this;
			ret += str;
			return ret;
		}
		lstr operator+(const lstr& str) const
		{
			lstr ret = *this;
			ret += str;
			return ret;
		}
		lstr operator+(const char c) const
		{
			lstr ret = *this;
			ret += c;
			return ret;
		}

		bool isEmpty() const { return size() == 0; }
		int32_t count() const { return (int32_t)size(); }
		char takeAt(int32_t offs)
		{
			char ret = c_str()[offs];
			erase(offs, 1);
			return ret;
		}

		int32_t indexOf(char el, int32_t first = 0, int32_t last = -1) const
		{
			if (first < 0)
				return -1;

			const char* str = c_str();
			size_t sz = size();

			if (last >= 0 && (size_t)last < sz)
				sz = last;

			for (size_t i = first; i < sz; i++)
			{
				if (str[i] == el)
					return (int32_t)i;
			}

			return -1;
		}

		// find a substring. Optionally starting at a given 'first' character and not including an
		// optional 'last' character. If last is -1 (the default), the whole string is searched
		int32_t find(const char* needle_str, size_t needle_len, int32_t first, int32_t last) const
		{
			// no default parameters for first/last here because otherwise it'd be dangerous with casting
			// and
			// misusing this instead of just specifying a 'first' below.
			const char* haystack = c_str();
			size_t haystack_len = size();

			if (first < 0)
				return -1;

			if (needle_len == 0)
				return 0;

			if (last >= 0 && (size_t)last < haystack_len)
				haystack_len = last;

			if ((size_t)first >= haystack_len)
				return -1;

			if (needle_len > haystack_len - first)
				return -1;

			for (size_t i = first; i <= haystack_len - needle_len; i++)
			{
				if (strncmp(haystack + i, needle_str, needle_len) == 0)
					return (int32_t)i;
			}

			return -1;
		}

		int32_t find(const char needle, int32_t first = 0, int32_t last = -1) const
		{
			if (first < 0)
				return -1;

			const char* haystack = c_str();
			size_t haystack_len = size();

			if (last >= 0 && (size_t)last < haystack_len)
				haystack_len = last;

			for (size_t i = first; i < haystack_len; i++)
			{
				if (haystack[i] == needle)
					return (int32_t)i;
			}

			return -1;
		}

		int32_t find(const lstr& needle, int32_t first = 0, int32_t last = -1) const
		{
			return find(needle.c_str(), needle.size(), first, last);
		}
		int32_t find(const char* needle, int32_t first = 0, int32_t last = -1) const
		{
			return find(needle, strlen(needle), first, last);
		}

		// find the first character that is in a given set of characters, from the start of the string
		// Optionally starting at a given 'first' character and not including an optional 'last'
		// character. If last is -1 (the default), the whole string is searched
		int32_t find_first_of(const lstr& needle_set, int32_t first = 0, int32_t last = -1) const
		{
			return find_first_last(needle_set, true, true, first, last);
		}
		// find the first character that is not in a given set of characters, from the start of the string
		int32_t find_first_not_of(const lstr& needle_set, int32_t first = 0, int32_t last = -1) const
		{
			return find_first_last(needle_set, true, false, first, last);
		}
		// find the first character that is in a given set of characters, from the end of the string
		int32_t find_last_of(const lstr& needle_set, int32_t first = 0, int32_t last = -1) const
		{
			return find_first_last(needle_set, false, true, first, last);
		}
		// find the first character that is not in a given set of characters, from the end of the string
		int32_t find_last_not_of(const lstr& needle_set, int32_t first = 0, int32_t last = -1) const
		{
			return find_first_last(needle_set, false, false, first, last);
		}

	private:
		int32_t find_first_last(const lstr& needle_set, bool forward_search, bool search_in_set,
			int32_t first, int32_t last) const
		{
			if (first < 0)
				return -1;

			const char* haystack = c_str();
			size_t haystack_len = size();

			if (last >= 0 && (size_t)last < haystack_len)
				haystack_len = last;

			if (forward_search)
			{
				for (size_t i = first; i < haystack_len; i++)
				{
					bool in_set = needle_set.contains(haystack[i]);
					if (in_set == search_in_set)
						return (int32_t)i;
				}
			}
			else
			{
				for (size_t i = 0; i < haystack_len; i++)
				{
					size_t idx = haystack_len - 1 - i;

					if (idx < (size_t)first)
						break;

					bool in_set = needle_set.contains(haystack[idx]);
					if (in_set == search_in_set)
						return (int32_t)idx;
				}
			}

			return -1;
		}

	public:
		bool contains(char needle) const { return indexOf(needle) != -1; }
		bool contains(const lstr& needle) const { return find(needle) != -1; }
		bool contains(const char* needle) const { return find(needle) != -1; }
		bool beginsWith(const lstr& beginning) const
		{
			if (beginning.length() > length())
				return false;

			return !strncmp(c_str(), beginning.c_str(), beginning.length());
		}
		bool endsWith(const lstr& ending) const
		{
			if (ending.length() > length())
				return false;

			return !strcmp(c_str() + length() - ending.length(), ending.c_str());
		}

		void removeOne(char el)
		{
			int idx = indexOf(el);
			if (idx >= 0)
				erase((size_t)idx, 1);
		}

		// remove any preceeding or trailing whitespace from the string, in-place
		void trim()
		{
			if (empty())
				return;

#define LSTR_IS_WHITESPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n' || (c) == '\0')

			const char* str = c_str();
			size_t sz = size();

			size_t start = 0;
			while (start < sz && LSTR_IS_WHITESPACE(str[start]))
				start++;

			// no non-whitespace characters, become the empty string
			if (start == sz)
			{
				clear();
				return;
			}

			size_t end = sz - 1;
			while (end > start && LSTR_IS_WHITESPACE(str[end]))
				end--;

			erase(end + 1, ~0U);
			erase(0, start);
		}

		// return a copy of the string with preceeding and trailing whitespace removed
		lstr trimmed() const
		{
			lstr ret = *this;
			ret.trim();
			return ret;
		}

		// for equality check with rdcstr, check quickly for empty string comparisons
		bool operator==(const lstr& o) const
		{
			if (o.size() == 0)
				return size() == 0;
			return !strcmp(o.c_str(), c_str());
		}

		// equality checks for other types, just check string directly
		bool operator==(const char* const o) const
		{
			if (o == NULL)
				return size() == 0;
			return !strcmp(o, c_str());
		}
		// for inverse check just reverse results of above
		bool operator!=(const char* const o) const { return !(*this == o); }
		bool operator!=(const lstr& o) const { return !(*this == o); }
		// define ordering operators
		bool operator<(const lstr& o) const { return strcmp(c_str(), o.c_str()) < 0; }
		bool operator>(const lstr& o) const { return strcmp(c_str(), o.c_str()) > 0; }
	};

	// macro that can append _lit to a macro parameter
#define STRING_LITERAL2(string) string##_lit
#define STRING_LITERAL(string) STRING_LITERAL2(string)

	inline lstr operator+(const char* const left, const lstr& right)
	{
		return lstr(left) += right;
	}

	inline bool operator==(const char* const left, const lstr& right)
	{
		return right == left;
	}

	inline bool operator!=(const char* const left, const lstr& right)
	{
		return right != left;
	}
} // namespace Luft