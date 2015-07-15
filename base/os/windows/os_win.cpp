#include "os_win.h"
#include <windows.h>
#include <wchar.h>
#include <assert.h>

int os::ListDir(const wchar_t *p_pDirName, wchar_t p_pFilenames[][MAX_PATH_LEN], int p_iMaxFilenames)
{
	wchar_t szAllFiles[MAX_PATH_LEN];
	int iLen = wcslen(p_pDirName);
	assert(iLen > 0 && iLen < MAX_PATH_LEN);

	if( p_pDirName[iLen] == L'\\' )
	{
		swprintf_s(szAllFiles, MAX_PATH_LEN, L"%ls*.*", p_pDirName);
	}
	else
	{
		swprintf_s(szAllFiles, MAX_PATH_LEN, L"%ls\\*.*", p_pDirName);
	}
	
	WIN32_FIND_DATA fileData;
	HANDLE hFiles = ::FindFirstFile(szAllFiles, &fileData);
	if( hFiles == INVALID_HANDLE_VALUE )
	{
		return -1;
	}
	int iFileCount = 0;
	do 
	{
		if ( fileData.cFileName[0] == L'.' )
		{
			continue;
		}
		if( (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{
			wcscpy_s(p_pFilenames[iFileCount], MAX_PATH_LEN, fileData.cFileName);
			iFileCount++;
		}
	} while ( ::FindNextFile(hFiles, &fileData) );

	::FindClose(hFiles);

	return iFileCount;
}

bool os::Remove( const wchar_t *p_pFileName )
{
	DWORD dwFileAttrs = ::GetFileAttributes(p_pFileName);
	if ( dwFileAttrs == 0xFFFFFFFF )
	{
		return false;
	}
	if ( dwFileAttrs & FILE_ATTRIBUTE_DIRECTORY )
	{
		return false;
	}
	BOOL bRet = ::DeleteFile(p_pFileName);
	if ( bRet == FALSE )
	{
		return false;
	}
	return true;
}
