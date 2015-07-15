#include "sa_utility.h"

namespace n_utility
{
	bool MakeName(const wchar_t *p_szOriginal, const wchar_t *p_szPrefix, const wchar_t *p_szSuffix, std::wstring &p_strName)
	{
		if( !p_szOriginal )
		{
			return false;
		}

		if( p_szPrefix )
		{
			p_strName = p_szPrefix;
		}
		else
		{
			p_strName.clear();
		}

		const wchar_t *p = p_szOriginal;
		while( *p != 0 )
		{
			if( *p != L' ' && *p != L'\r' && *p != L'\n' && *p != L'\\' && *p != L':' && *p != L'.' && *p != L'/' && *p != L'\t' )
			{
				p_strName += *p;
			}
			else
			{
				p_strName += L'_';
			}
			++p;
		}
		if( p_szSuffix )
		{
			p_strName += p_szSuffix;
		}

		return true;
	}
}