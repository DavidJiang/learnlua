// author: helitao
#include "string_win.h"
#include <windows.h>
#include <stdlib.h>

namespace std
{
	wstring &operator+=(wstring &lhs, const string &rhs)
	{
		return lhs += rhs.c_str();
	}

	wstring &operator+=(wstring &lhs, int rhs)
	{
		wchar_t wstr[16] = {0};
		swprintf_s(wstr, 16, L"%d", rhs);

		lhs += wstr;

		return lhs;
	}

	wstring &operator+=(wstring &lhs, const char *rhs)
	{
		if( !rhs )
		{
			return lhs;
		}

		int nlen = ::MultiByteToWideChar(936, 0, rhs, -1, 0, 0);
		wchar_t *pwstr = new wchar_t[nlen + 1];
		nlen = ::MultiByteToWideChar(936, 0, rhs, -1, pwstr, nlen + 1);
		pwstr[nlen] = 0;

		lhs += pwstr;
		delete[] pwstr;

		return lhs;
	}

	wstring &operator+=(wstring &lhs, float rhs)
	{
		wchar_t wstr[32] = {0};
		swprintf_s(wstr, 32, L"%f", rhs);

		lhs += wstr;

		return lhs;
	}

	wstring &operator+=(wstring &lhs, unsigned int rhs)
	{
		wchar_t wstr[16] = {0};
		swprintf_s(wstr, 16, L"%d", rhs);

		lhs += wstr;

		return lhs;
	}

	const wchar_t *c_str(const wstring &rhs)
	{
		return rhs.c_str();
	}
}