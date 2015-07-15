#ifndef _WSTRING_LINUX_H_
#define _WSTRING_LINUX_H_

#include "wchar_linux.h"

#include <string>
//#include <iosfwd>
#include <sstream>
#include <assert.h>
#include <iconv.h>
#include <string.h>
#include <stdio.h>

namespace std
{
	struct wchar_traits
	{
		typedef wchar_t           char_type;
		typedef wint_t            int_type;
		typedef streamoff         off_type;
		typedef wstreampos        pos_type;
		typedef mbstate_t         state_type;

		static void
			assign(char_type& __c1, const char_type& __c2)
		{ __c1 = __c2; }

		static bool
			eq(const char_type& __c1, const char_type& __c2)
		{ return __c1 == __c2; }

		static bool
			lt(const char_type& __c1, const char_type& __c2)
		{ return __c1 < __c2; }

		static int
			compare(const char_type* __s1, const char_type* __s2, size_t __n)
		{ return wmemcmp_emulation(__s1,__s2, __n); }

		static size_t
			length(const char_type* __s)
		{ return wcslen_emulation(__s); }

		static const char_type*
			find(const char_type* __s, size_t __n, const char_type& __a)
		{ return (char_type *)wmemchr_emulation(__s, __a, __n); }

		static char_type*
			move(char_type* __s1, const char_type* __s2, size_t __n)
		{ return (char_type *)wmemmove_emulation(__s1,__s2, __n); }

		static char_type*
			copy(char_type* __s1, const char_type* __s2, size_t __n)
		{ return (char_type *)wmemcpy_emulation(__s1, __s2, __n); }

		static char_type*
			assign(char_type* __s, size_t __n, char_type __a)
		{ return (char_type*)wmemset_emulation(__s, __a, __n); }

		static char_type
			to_char_type(const int_type& __c)
		{ return char_type(__c); }

		static int_type
			to_int_type(const char_type& __c)
		{ return int_type(__c); }

		static bool
			eq_int_type(const int_type& __c1, const int_type& __c2)
		{ return __c1 == __c2; }

		static int_type
			eof()
		{ return static_cast<int_type>(WEOF); }

		static int_type
			not_eof(const int_type& __c)
		{ return eq_int_type(__c, eof()) ? 0 : __c; }
	};

//	template<> struct char_traits<t_wchar>; 
//	typedef basic_string<t_wchar> t_wstring; 
	typedef basic_string<wchar_t,wchar_traits, allocator<wchar_t> > t_wstring;

   	const t_wstring wstrNull(L"");

	inline	const wchar_t *c_str(const t_wstring &left)
	{
		return left.c_str();
	}
#define MAX_RIGHT_LENGTH  1024
	
	inline	t_wstring & operator+=(
		t_wstring &left,
		const char *right)
	{
		size_t srclen = strlen(right);
		assert( srclen < MAX_RIGHT_LENGTH && srclen > 0 );
		wchar_t wright[MAX_RIGHT_LENGTH] = {0};
		iconv_t convd = iconv_open("UCS-2","utf-8");
		if( convd == (iconv_t) -1)
		{
			assert(false);
			return left;
		}
		size_t inlen = srclen;
		char *psrc = const_cast<char*>(right);
		size_t outlen = MAX_RIGHT_LENGTH;
		char *pdst = reinterpret_cast<char*>(wright);
		assert( psrc != NULL );
		size_t ret = iconv( convd, &psrc, &inlen, &pdst, &outlen );
		if( ret == (size_t)-1 )
		{
			assert(false);
			return left;
		}
		
		iconv_close(convd);
		return (left +=wright);
	}
	inline 	t_wstring operator+(
		const t_wstring &left,
		const char *right
		)
	{
		t_wstring strtmp(left);
		strtmp += right;
		return strtmp;
	}
#define MAX_PRINT_BUFFER_LEN 8 * 1024
	inline int wPrintf(const wchar_t *buffer)
	{
		assert( buffer != NULL && buffer[0] != 0 );
		iconv_t convd = iconv_open("utf-8", "UCS-2");
		if( convd == (iconv_t)-1 )
		{
			assert(false);
			return -1;
		}

		char *pSrc = const_cast<char*>(reinterpret_cast<const char*>(buffer));
		if( pSrc == NULL )
		{
			assert(false);
			return -1;
		}
		size_t srclen = _tcslen( buffer ) * sizeof(wchar_t);
		assert(srclen > 0 && srclen < MAX_PRINT_BUFFER_LEN);
		size_t oldlen = srclen;
		char szDest[MAX_PRINT_BUFFER_LEN] = {0};
		size_t dstlen = MAX_PRINT_BUFFER_LEN;
		char *pDst = szDest;
		size_t ret = iconv(convd, &pSrc, &srclen, &pDst, &dstlen);
		if( ret == (size_t)-1 )
		{
			assert(false);
			return -1;
		}
		printf("%s\n", szDest);

		iconv_close(convd);
		return (oldlen - srclen)/2;
	}
#undef wprintf
#define wprintf wPrintf

}
#endif
