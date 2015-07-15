// author: helitao
#include "filemap_win.h"
#include <assert.h>
#include "access_win.h"

t_filemap::t_filemap() : m_szLastError(0), m_hFilemap(0), m_nSize(0), m_pData(0), m_bCreator(false)
{
	m_state = ec_uninitiated;
}

t_filemap::~t_filemap()
{
	Close();
}

bool t_filemap::Open(const wchar_t *p_szFilePath, const wchar_t *p_pShmName)
{
	if( !p_szFilePath )
	{
		m_szLastError = L"参数错误";
		return false;
	}

	// std::wstring strName;
	// std::wstring strSuffix = L"_FilemapName";
	// if( p_szSuffix )
	// {
	// 	strSuffix += L"_";
	// 	strSuffix += p_szSuffix;
	// }
	// MakeName(p_szFilePath, L"Local\\", strSuffix.c_str(), strName);

	if( p_pShmName != NULL )
	{
		m_hFilemap = ::OpenFileMapping(FILE_MAP_READ, FALSE, p_pShmName);
		if( !p_szFilePath && !m_hFilemap )
		{
			m_szLastError = L"打开文件映射失败";
			return false;
		}
	}

	HANDLE hFile = INVALID_HANDLE_VALUE;
	if( !m_hFilemap )
	{
		if( !p_szFilePath )
		{
			m_szLastError = L"文件路径错误";
			return false;
		}

		hFile = ::CreateFile(p_szFilePath, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( INVALID_HANDLE_VALUE == hFile )
		{
			m_szLastError = L"打开文件失败";
			return false;
		}

		m_nSize = (int)::GetFileSize(hFile, 0);
		m_hFilemap = ::CreateFileMapping(hFile, n_access::GetDefaultSecurity(), PAGE_READONLY, 0, m_nSize, p_pShmName);
		::CloseHandle(hFile);
		hFile = 0;

		if( !m_hFilemap )
		{
			m_szLastError = L"创建文件映射失败";
			return false;
		}

		m_bCreator = true;
	}

	m_pData = (unsigned char *)::MapViewOfFile(m_hFilemap, FILE_MAP_READ, 0, 0, 0);
	if( !m_pData )
	{
		m_szLastError = L"映射文件失败";
		if( m_hFilemap )
		{
			::CloseHandle(m_hFilemap);
			m_hFilemap = 0;
		}
		if( hFile != INVALID_HANDLE_VALUE )
		{
			::CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
		return false;
	}

	m_szLastError = 0;
	m_strName = p_pShmName;
	m_state = ec_mapped;

	return true;
}

bool t_filemap::Open(const std::wstring &p_strFilePath, const std::wstring &p_strShmName)
{
	return Open(p_strFilePath.c_str(), p_strShmName.c_str());
}

bool t_filemap::Open(const t_path &p_path, const std::wstring &p_strShmName)
{
	return Open(p_path.FullPath().c_str(), p_strShmName.c_str());
}

 bool t_filemap::OpenNC(const wchar_t *p_szName)
 {
 	assert(p_szName != NULL);
 	if(NULL == p_szName){
 		return false;
 	}

 	m_hFilemap = ::OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, p_szName);
 	if( !m_hFilemap ){
 		return false;
 	}

 	m_pData = (unsigned char *)::MapViewOfFile(m_hFilemap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
 	if( !m_pData ){
 		m_szLastError = L"映射文件失败";
 		if( m_hFilemap ){
 			::CloseHandle(m_hFilemap);
 			m_hFilemap = 0;
 		}
 		return false;
 	}

 	m_nSize = 0;
 	m_szLastError = 0;
 	m_strName = p_szName;
 	m_state = ec_mapped;
 	return true;
 }

bool t_filemap::Open(const wchar_t *p_szName, int p_nSize)
{
	if( p_nSize < 0 )
	{
		m_szLastError = L"参数错误";
		return false;
	}

	if( p_szName )
	{
		m_hFilemap = ::OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, p_szName);
	}

	if( !m_hFilemap )
	{
		m_hFilemap = ::CreateFileMapping(INVALID_HANDLE_VALUE, n_access::GetDefaultSecurity(), PAGE_READWRITE, 0, p_nSize, p_szName);
		if( !m_hFilemap )
		{
			m_szLastError = L"创建文件映射失败";
			return false;
		}
		m_bCreator = true;
	}

	m_pData = (unsigned char *)::MapViewOfFile(m_hFilemap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, p_nSize);
	if( !m_pData )
	{
		m_szLastError = L"映射文件失败";
		if( m_hFilemap )
		{
			::CloseHandle(m_hFilemap);
			m_hFilemap = 0;
		}
		return false;
	}

	m_nSize = p_nSize;
	m_szLastError = 0;
	m_strName = p_szName;
	m_state = ec_mapped;
	return true;
}

bool t_filemap::Open(const std::wstring &p_strName, int p_nSize)
{
	return Open(p_strName.c_str(), p_nSize);
}

bool t_filemap::IsOpen()
{
	if( m_state == ec_mapped )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool t_filemap::IsCreator()
{
	return m_bCreator;
}

bool t_filemap::Close()
{
	if( m_pData )
	{
		::UnmapViewOfFile(m_pData);
		m_pData = 0;
	}

	if( m_hFilemap )
	{
		::CloseHandle(m_hFilemap);
		m_hFilemap = 0;
	}

	m_nSize = 0;
	m_strName = L"";
	m_szLastError = 0;
	m_bCreator = false;
	m_state = ec_uninitiated;
	return true;
}

int t_filemap::GetSize()
{
	return m_nSize;
}

unsigned char *t_filemap::GetDataPtr()
{
	return m_pData;
}

const wchar_t *t_filemap::GetLastError()
{
	return m_szLastError;
}

bool t_filemap::MakeName(const wchar_t *p_szOriginal, const wchar_t *p_szPrefix, const wchar_t *p_szSuffix, std::wstring &p_strName)
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

bool t_filemap::Sync(void *p_pAddr, size_t p_length)
{
	if ( p_pAddr == NULL )
	{
		p_pAddr = m_pData;
		p_length = m_nSize;
	}
	BOOL bRet = ::FlushViewOfFile(p_pAddr, p_length);
	if ( bRet == FALSE )
	{
		assert(false);
		return false;
	}
	return true;
}