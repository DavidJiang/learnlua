#include "path_linux.h"
#include <iconv.h>
#include <wchar.h>
#include <assert.h>
#include <string.h>

const static unsigned int MAX_PATH_LEN = 1024;
static const  wchar_t PATH_SEPARATOR = L'/';

t_path::t_path()
{
	m_strPath = L"";
}

t_path::t_path(const wchar_t *p_pFilePath)
{
	m_strPath = p_pFilePath;
}

t_path::t_path( const wchar_t *p_pFirstPath,
		const wchar_t *p_pLastPath)
{
	m_strPath = p_pFirstPath;
	m_strPath += PATH_SEPARATOR;
	m_strPath += p_pLastPath;
	NormPath();
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
	NormPath();
}

t_path::t_path(const t_path &p_path)
{
	m_strPath = p_path.m_strPath;
	m_pathInfo = p_path.m_pathInfo;
}

void t_path::operator=( const wchar_t *p_pFilePath )
{
	m_strPath = p_pFilePath;
}

const wstring t_path::DirName(void) const 
{
	if( m_pathInfo.first.length() != 0 )
	{
		return m_pathInfo.first;
	}
	wstring::size_type pos = m_strPath.find_last_of(PATH_SEPARATOR);
	if( pos == wstring::npos )
	{
		return L"";
	}
	return m_strPath.substr(0, pos);
}

const wstring t_path::BaseName(void) const
{
	if( m_pathInfo.second.length() != 0 )
	{
		return m_pathInfo.second;
	}
	wstring::size_type pos = m_strPath.find_last_of(PATH_SEPARATOR);
	wstring::size_type len = m_strPath.length();
	if( pos == wstring::npos || pos == (len-1) )
	{
		return  wstring(L"");
	}
	wstring::size_type count = len - pos -1;
	return m_strPath.substr(pos+1, count);
}

void t_path::NormPath(void) const
{
	//replace \\ with / under linux.
	for( size_t i = 0; i < m_strPath.length(); i++ )
	{
		if( m_strPath[i] == L'\\' )
		{
			m_strPath[i] = PATH_SEPARATOR;
		}
	}
	//unique path separators
	wchar_t szPath[MAX_PATH_LEN] = {0};
	unsigned int nLen = m_strPath.length();
	assert( nLen < MAX_PATH_LEN );
	wcsncpy(szPath, m_strPath.c_str(), nLen);

	wchar_t *pStart = szPath;
	wchar_t *pEnd = NULL;
	unsigned int nMoveLen = 0;
	while( *pStart != 0 )
	{
		pStart = wcschr(pStart, PATH_SEPARATOR);
		if( pStart  == NULL )
		{
			break;
		}
		nLen = wcslen(pStart);
		
		pEnd = pStart + 1;
		//find the end separator.
		if ( *pEnd == PATH_SEPARATOR )
		{
            nMoveLen = pEnd - pStart;
            nMoveLen = nLen - nMoveLen + 1;
            wmemmove(pStart, pEnd, nMoveLen);
        }
		pStart++;
	}
	m_strPath = szPath;
}

void t_path::Join(const wchar_t *p_pDirName, const wchar_t *p_pBaseName)
{
	m_strPath = L"";
	m_strPath += p_pDirName;

	if( m_strPath[m_strPath.length() -1] != PATH_SEPARATOR &&
		p_pBaseName[0] != PATH_SEPARATOR )
	{
		m_strPath += PATH_SEPARATOR;
	}
	m_strPath += p_pBaseName;
}

pair<wstring, wstring> & t_path::Split(void)
{
	wstring::size_type pos = m_strPath.find_last_of(PATH_SEPARATOR);
	wstring::size_type len = m_strPath.length();
	if( pos == wstring::npos )
	{
		return  m_pathInfo;
	}
	wstring::size_type count = len - pos -1;
	m_pathInfo.first = m_strPath.substr(0, pos);
	if( count != 0 )
	{
		m_pathInfo.second = m_strPath.substr(pos+1, count);
	}
	return m_pathInfo;
}


t_path & t_path::operator+=(const wchar_t *p_path)
{
	m_strPath += PATH_SEPARATOR;
	m_strPath += p_path;
	NormPath();
	return *this;
}
