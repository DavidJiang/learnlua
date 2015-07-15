#ifndef _SA_UTILITY_H_
#define _SA_UTILITY_H_

#include <string>

namespace n_utility
{
	bool MakeName(const wchar_t *p_szOriginal, const wchar_t *p_szPrefix, const wchar_t *p_szSuffix, std::wstring &p_strName);
}

#endif // _SA_UTILITY_H_
