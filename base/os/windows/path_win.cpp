// author: helitao
#include "path_win.h"

const static unsigned int MAX_PATH_LEN = 1024;
static const wchar_t *PATH_SEPARATOR = L"\\";

// \ / : * ? " < > |
t_path::t_path()
{

}

t_path::t_path(const t_path &p_oPath)
{
	m_strPath = p_oPath.FullPath();
}

t_path::t_path(const std::wstring &p_strPath)
{
	m_strPath = p_strPath;
	Normalize();
}

t_path::t_path(const wchar_t *p_szPath)
{
	m_strPath = p_szPath;
	Normalize();
}

t_path::t_path( const wchar_t *p_pFirstPath,
		const wchar_t *p_pLastPath)
{
	m_strPath = p_pFirstPath;
	m_strPath += PATH_SEPARATOR;
	m_strPath += p_pLastPath;
	Normalize();
}

t_path::t_path(const wchar_t *p_pFirstPath, 
			   const wchar_t *p_pSecondPath,
			   const wchar_t *p_pLastPath)
{
	m_strPath = p_pFirstPath;
	m_strPath += PATH_SEPARATOR;
	m_strPath += p_pSecondPath;
	m_strPath += PATH_SEPARATOR;
	m_strPath += p_pLastPath;
	Normalize();
}

t_path::~t_path()
{

}

t_path &t_path::operator=(const t_path &p_oPath)
{
	m_strPath = p_oPath.FullPath();

	return *this;
}

t_path &t_path::operator=(const std::wstring &p_strPath)
{
	m_strPath = p_strPath;
	Normalize();

	return *this;
}

t_path &t_path::operator=(const wchar_t *p_szPath)
{
	m_strPath = p_szPath;
	Normalize();

	return *this;
}

t_path &t_path::operator+=(const t_path &p_oPath)
{
	m_strPath += L"\\";
	m_strPath += p_oPath.FullPath();

	return *this;
}

t_path &t_path::operator+=(const std::wstring &p_strPath)
{
	return operator+=(p_strPath.c_str());
}

t_path &t_path::operator+=(const wchar_t *p_szPath)
{
	if( p_szPath && *p_szPath )
	{
		m_strPath += L"\\";
		m_strPath += p_szPath;
		Normalize();
	}

	return *this;
}

void t_path::NormPath() const
{
	//todo:
}

const std::wstring t_path::DirName() const
{
	if( m_strPath.length() <= 0 )
	{
		return L"";
	}

	std::wstring::size_type pos = m_strPath.rfind(L'\\', m_strPath.length());
	if( std::wstring::npos == pos || pos == 0 )
	{
		return m_strPath;
	}

	// \\ 开头
	if( pos == 1 && m_strPath.c_str()[0] == L'\\' )
	{
		return m_strPath;
	}
	
	int nLen = (int)pos;
	wchar_t *pszDir = new wchar_t[nLen + 1];
	wcsncpy_s(pszDir, nLen + 1, m_strPath.c_str(), nLen);

	std::wstring strDir = pszDir;
	delete[] pszDir;

	return strDir;
}

const std::wstring t_path::BaseName() const
{
	if( m_strPath.length() <= 0 )
	{
		return L"";
	}

	std::wstring::size_type pos = m_strPath.rfind(L'\\', m_strPath.length());
	if( std::wstring::npos == pos || pos == 0 )
	{
		return L"";
	}

	// \\ 开头
	if( pos == 1 && m_strPath.c_str()[0] == L'\\' )
	{
		return L"";
	}

	std::wstring strDir = m_strPath.c_str() + pos + 1;

	return strDir;
}

const std::wstring &t_path::FullPath() const
{
	return m_strPath;
}

void t_path::Normalize()
{
	// windows下，以\\开头的文件路径也是合法的

	int nLen = (int)m_strPath.length();
	wchar_t *pszPath = new wchar_t[nLen + 1];
	int nRealLen = 0;
	bool bPrevIsSlash = false;
	int nSlashHead = 0;

	for ( int i = 0; i < nLen; ++i )
	{
		if( m_strPath.c_str()[i] == L'\\' || m_strPath.c_str()[i] == L'/' )
		{
			if( !bPrevIsSlash )
			{
				if( nSlashHead >= 0 && nSlashHead < 2 )
				{
					pszPath[nRealLen++] = L'\\';
					++nSlashHead;
					if( nSlashHead == 2 )
					{
						bPrevIsSlash = true;
					}
				}
				else
				{
					pszPath[nRealLen++] = L'\\';
					bPrevIsSlash = true;
				}
			}
		}
		else
		{
			nSlashHead = -1;
			bPrevIsSlash = false;
			pszPath[nRealLen++] = m_strPath.c_str()[i];
		}
	}

	pszPath[nRealLen] = 0;

	// 去掉头尾的 \ 
	int nHeadLen = 0;

	if( nRealLen > 1 && pszPath[0] == L'\\' && pszPath[1] != L'\\' )
	{
		nHeadLen = 1;
	}

	if( nRealLen > 0 && pszPath[nRealLen - 1] == L'\\' )
	{
		pszPath[nRealLen - 1] = 0;
	}

	m_strPath = pszPath + nHeadLen;
	delete[] pszPath;
}