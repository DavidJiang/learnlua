// author: helitao
#include "file_win.h"
#include <sys/stat.h>
#include <stdio.h>
#include "access_win.h"

t_file::t_openParam t_file::s_openParams[] = 
{
	{ec_in, OPEN_EXISTING, FILE_SHARE_READ, FILE_READ_DATA},
	{ec_out, CREATE_ALWAYS, 0, FILE_WRITE_DATA | READ_CONTROL | WRITE_DAC | WRITE_OWNER},
	{ec_app, OPEN_ALWAYS, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_APPEND_DATA | READ_CONTROL | WRITE_DAC | WRITE_OWNER},
	{ec_in|ec_out, OPEN_ALWAYS, 0, FILE_READ_DATA | FILE_WRITE_DATA | READ_CONTROL | WRITE_DAC | WRITE_OWNER},
	{ec_trunc, TRUNCATE_EXISTING , 0, FILE_WRITE_DATA | READ_CONTROL | WRITE_DAC | WRITE_OWNER},
	{ec_invalid, 0, 0, 0},
};

t_file::t_file() : m_hFile(INVALID_HANDLE_VALUE), m_szLastError(0), m_openMode(ec_invalid)
{

}

t_file::t_file(const wchar_t *p_fileName, int p_mode)
{
	Open(p_fileName, p_mode);
}

t_file::t_file(const t_path &p_path, int p_mode)
{
	Open(p_path.FullPath().c_str(), p_mode); 
}

t_file::t_file(const std::wstring &p_fileName, int p_mode)
{
	Open(p_fileName, p_mode);
}

t_file::~t_file()
{

}

bool t_file::Open(const wchar_t *p_fileName, int p_mode)
{
	if( !p_fileName )
	{
		m_szLastError = L"文件名错误";
		return false;
	}

	t_openParam *pOpenParam = GetOpenParam(p_mode);
	if( !pOpenParam )
	{
		m_szLastError = L"文件打开方式错误";
		return false;
	}

	int nRetry = 0;
	while ( nRetry < 3 )
	{
		m_hFile = ::CreateFile(p_fileName, pOpenParam->m_dwAccess, pOpenParam->m_dwShare, NULL, pOpenParam->m_dwCreate, FILE_ATTRIBUTE_NORMAL, NULL);
		if( m_hFile == INVALID_HANDLE_VALUE )
		{
			DWORD dwErr = ::GetLastError();
			if( dwErr == 32 )
			{
				m_szLastError = L"文件被占用";
				::Sleep(100);
				++nRetry;
				continue;
			}
			else if( dwErr == 5 ) 
			{
				m_szLastError = L"文件只读";
				::SetFileAttributes(p_fileName, FILE_ATTRIBUTE_NORMAL);
				++nRetry;
				if( nRetry == 3 && p_mode == ec_out )
				{
					nRetry = 0;
					p_mode = ec_trunc;
				}
				continue;
			}

			if( dwErr == 0 )
			{
				m_szLastError = L"文件不存在";
				return false;
			}

			m_szLastError = L"打开文件错误：未知错误";
			return false;
		}
		break;
	}

	if( m_hFile == INVALID_HANDLE_VALUE || m_hFile == 0 )
	{
		m_hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	m_strPath = p_fileName;
	m_openMode = p_mode;
	m_szLastError = 0;

	return true;
}

bool t_file::Open(const std::wstring &p_fileName, int p_mode)
{
	return Open(p_fileName.c_str(), p_mode);
}

bool t_file::Open(const t_path &p_fileName, int p_mode)
{
	return Open(p_fileName.FullPath().c_str(), p_mode);
}

bool t_file::Close()
{
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		m_szLastError = L"文件未打开";
		return false;
	}

	if( m_openMode != ec_in )
	{
		n_access::AddAccessRight(m_hFile, SE_FILE_OBJECT, GENERIC_ALL);
		std::wstring strName = m_strPath.BaseName();
		//const wchar_t *szName = m_strPath.BaseName().c_str();
		bool bExe = false;
		const wchar_t *pExt = wcsrchr(strName.c_str(), L'.');
		if( pExt && _wcsicmp(pExt, L".exe") == 0 )
		{
			bExe = true;
		}
		if( !bExe )
		{
			n_access::SetLowLabel(m_hFile, SE_FILE_OBJECT);
		}
	}

	::CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
	m_strPath = L"";
	m_openMode = ec_invalid;
	m_szLastError = 0;

	return true;
}

bool t_file::Read(unsigned char *p_szBuf, int p_nSize, int &p_nRead)
{
	if( !p_szBuf && p_nSize <= 0 )
	{
		m_szLastError = L"参数错误";
		return false;
	}

	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		m_szLastError = L"文件未打开";
		return false;
	}

	DWORD dwRead = 0;
	if( !::ReadFile(m_hFile, p_szBuf, p_nSize, &dwRead, NULL) )
	{
		m_szLastError = L"读文件错误";
		return false;
	}

	if( dwRead == 0 )
	{
		m_szLastError = L"文件已至末尾";
		return false;
	}

	p_nRead = (int)dwRead;
	m_szLastError = 0;

	return true;
}

bool t_file::Write(const unsigned char *p_szBuf, int p_nSize, int &p_nWritten)
{
	if( !p_szBuf && p_nSize <= 0 )
	{
		m_szLastError = L"参数错误";
		return false;
	}

	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		m_szLastError = L"文件未打开";
		return false;
	}

	DWORD dwWritten = 0;
	if( !::WriteFile(m_hFile, p_szBuf, p_nSize, &dwWritten, NULL) )
	{
		m_szLastError = L"写文件错误";
		return false;
	}

	p_nWritten = (int)dwWritten;
	m_szLastError = 0;

	return true;
}

bool t_file::Flush()
{
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		m_szLastError = L"文件未打开";
		return false;
	}

	if ( !::FlushFileBuffers(m_hFile) )
	{
		m_szLastError = L"回写文件错误";
		return false;
	}

	m_szLastError = 0;

	return true;
}

bool t_file::Seek(int p_nPos, e_whenceMode p_origin)
{
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		m_szLastError = L"文件未打开";
		return false;
	}

	if( INVALID_SET_FILE_POINTER == ::SetFilePointer(m_hFile, p_nPos, 0, (DWORD)p_origin) )
	{
		m_szLastError = L"移动文件游标错误";
		return false;
	}

	m_szLastError = 0;

	return true;
}

int t_file::GetCurPos()
{
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		m_szLastError = L"文件未打开";
		return -1;
	}

	DWORD dwPos = ::SetFilePointer(m_hFile, 0, 0, FILE_CURRENT);
	if( dwPos == INVALID_SET_FILE_POINTER )
	{
		m_szLastError = L"获取文件游标错误";
		return -1;
	}

	m_szLastError = 0;

	return (int)dwPos;
}

bool t_file::IsOpen()
{
	m_szLastError = 0;

	return (m_hFile != INVALID_HANDLE_VALUE);
}

int t_file::GetSize()
{
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		m_szLastError = L"文件未打开";
		return 0;
	}

	DWORD dwSizeLow = 0, dwSizeHigh = 0;
	dwSizeLow = ::GetFileSize(m_hFile, &dwSizeHigh);
	if( dwSizeLow == INVALID_FILE_SIZE )
	{
		if( ::GetLastError() == NO_ERROR )
		{
			m_szLastError = 0;
			return 0;
		}
		else
		{
			m_szLastError = L"获取文件大小错误";
			return -1;
		}
	}

	return (int)dwSizeLow;
}

t_file::t_openParam *t_file::GetOpenParam(int p_mode)
{
	t_openParam *pParams = s_openParams;
	while( pParams->m_cOpenMode != ec_invalid )
	{
		if( (*pParams).m_cOpenMode == p_mode )
		{
			return pParams;
		}
		++pParams;
	}

	return 0;
}

const wchar_t *t_file::GetLastError()
{
	return m_szLastError;
}

const t_path &t_file::GetPath()
{
	return m_strPath;
}

bool t_fileUtil::FileExists(const t_path &p_path)
{
	//todo: find a better way.
	struct _stat64i32 sbuf;
	int ret = _wstat( p_path.FullPath().c_str(), &sbuf);

	if( ret == -1 )
	{
		char szError[32] = {0};
		strerror_s(szError, 32, errno);
		perror( szError );
		return false;
	}
	return true;
}

bool t_fileUtil::FolderExists(const t_path &p_path)
{
	//todo: verify
	struct _stat64i32 sbuf;
	int ret = _wstat( p_path.FullPath().c_str(), &sbuf);

	if( ret == -1 )
	{
		char szError[32] = {0};
		strerror_s(szError, 32, errno);
		perror( szError );
		return false;
	}

	if((sbuf.st_mode & _S_IFDIR) != 0)
	{
		return true;
	}

	return false;
}

bool t_fileUtil::GetFileSize(const t_path &p_path, size_t &p_fileSize)
{
	struct _stat64i32 sbuf;
	int ret = _wstat( p_path.FullPath().c_str(), &sbuf);
	if( ret == -1 )
	{
		char szError[32] = {0};
		strerror_s(szError, 32, errno);
		perror( szError );
		return false;
	}
	p_fileSize = sbuf.st_size;
	return true;
}

bool t_fileUtil::RemoveFile(const t_path &p_path)
{
	int ret = _wremove(p_path.FullPath().c_str() );
	if( ret == -1 )
	{
		char szError[32] = {0};
		strerror_s(szError, 32, errno);
		perror( szError );
		return false;
	}
	return true;
}

bool t_fileUtil::MakeDir(const t_path &p_path)
{
	//todo:change mode according the needs.
	int ret = _wmkdir( p_path.FullPath().c_str() );
	if( ret == -1 )
	{
		char szError[32] = {0};
		strerror_s(szError, 32, errno);
		perror( szError );
		return false;
	}
	return true;
}

bool t_fileUtil::RemoveDir(const t_path &p_path)
{
	int ret = _wrmdir( p_path.FullPath().c_str() );
	if( ret == -1 )
	{
		char szError[32] = {0};
		strerror_s(szError, 32, errno);
		perror( szError );
		return false;
	}
	return true;
}

bool t_fileUtil::CopyFile(const t_path &p_fromPath, const t_path &p_toPath)
{
	BOOL bRet = ::CopyFileW(p_fromPath.FullPath().c_str(), p_toPath.FullPath().c_str(), FALSE );
	if( bRet == 0 )
	{
		DWORD dwError = GetLastError();
		char szError[32] = {0};
		strerror_s(szError, 32, errno);
		perror( szError );
		return false;
	}
	return true;
}

bool t_fileUtil::MoveFile(const t_path &p_fromPath, const t_path &p_toPath)
{
	BOOL bRet = ::MoveFileExW(p_fromPath.FullPath().c_str(), p_toPath.FullPath().c_str(), MOVEFILE_REPLACE_EXISTING);
		//::MoveFileW(p_fromPath.FullPath().c_str(), p_toPath.FullPath().c_str() );
	if( bRet == 0 )
	{
		DWORD dwError = GetLastError();
		char szError[32] = {0};
		strerror_s(szError, 32, errno);
		perror( szError );
		return false;
	}
	return true;
}

bool t_fileUtil::GetTempPath( t_path &p_pathTemp )
{
	TCHAR szTmpPath[MAX_PATH] = {0};
	if ( !::GetTempPath(MAX_PATH, szTmpPath) )
	{
		return false;
	}
	TCHAR szTmpFileName[MAX_PATH] = {0};
	if ( !::GetTempFileName(szTmpPath, NULL, 0, szTmpFileName) )
	{
		return false;
	}
	p_pathTemp = szTmpFileName;
	return true;
}


//
// t_fileEnum
//
t_fileEnum::t_fileEnum(const wchar_t *p_szDir, bool p_bRecursive, const wchar_t *p_szFilter)
{
	if( !p_szDir )
	{
		return;
	}

	EnumFiles(p_szDir, p_bRecursive, p_szFilter);
}

t_fileEnum::~t_fileEnum()
{
	m_vFiles.clear();
}

int t_fileEnum::GetFileCount()
{
	return (int)m_vFiles.size();
}

const wchar_t *t_fileEnum::GetFileName(int p_nIndex)
{
	if( p_nIndex < 0 || p_nIndex >= (int)m_vFiles.size() )
	{
		return 0;
	}

	return m_vFiles[p_nIndex].c_str();
}

void t_fileEnum::EnumFiles(const wchar_t *p_szDir, bool p_bRecursive, const wchar_t *p_szFilter)
{
	t_path path = p_szDir;
	if( p_szFilter )
	{
		path += p_szFilter;
	}
	else
	{
		path += L"*.*";
	}

	WIN32_FIND_DATA wfd = {0};
	HANDLE hFind = ::FindFirstFile(path.FullPath().c_str(), &wfd);
	BOOL bFindRes = hFind == INVALID_HANDLE_VALUE ? FALSE : TRUE;
	while ( bFindRes )
	{
		if( wcscmp(wfd.cFileName, L".") == 0 && wcscmp(wfd.cFileName, L"..") == 0)
		{
		}
		else if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( p_bRecursive )
			{
				t_path subPath = p_szDir;
				subPath += wfd.cFileName;
				EnumFiles(subPath.FullPath().c_str(), p_bRecursive, p_szFilter);
			}
		}
		else
		{
			t_path filePath = p_szDir;
			filePath += wfd.cFileName;
			m_vFiles.push_back(filePath.FullPath());
		}

		bFindRes = ::FindNextFile(hFind, &wfd);
	}

	::FindClose(hFind);
}