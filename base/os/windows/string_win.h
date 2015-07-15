// author: helitao
#ifndef _STRING_WIN_H_
#define _STRING_WIN_H_

#include <string>

namespace std
{
	wstring &operator+=(wstring &lhs, const string &rhs);
	wstring &operator+=(wstring &lhs, int rhs);
	wstring &operator+=(wstring &lhs, const char *rhs);
	wstring &operator+=(wstring &lhs, float rhs);
	wstring &operator+=(wstring &lhs, unsigned int rhs);
}

#endif	// _STRING_WIN_H_