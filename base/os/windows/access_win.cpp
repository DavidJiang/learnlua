// author: helitao
#include "access_win.h"
#include <Windows.h>
#include <sddl.h> 
#include <sys/stat.h>

#pragma comment(lib, "Version.lib")

namespace n_access
{
#define SECURITY_APP_PACKAGE_AUTHORITY {0x00, 0x00, 0x00, 0x00, 0x00, 0x0F}
#define SECURITY_APP_PACKAGE_BASE_RID (0x00000002L)
#define SECURITY_BUILTIN_PACKAGE_ANY_PACKAGE (0x00000001L)
#ifndef LABEL_SECURITY_INFORMATION
#define LABEL_SECURITY_INFORMATION 0x00000010L
#endif
#define LOW_INTEGRITY_SDDL_SACL L"S:(ML;;NW;;;LW)"
#define MEDIUM_INTEGRITY_SDDL_SACL L"S:(ML;;NW;;;ME)"

	bool GetFileVersion(const wchar_t *szFilePath, WORD (&dwVersions)[4])
	{
		DWORD dwInfoSize = 0;
		dwInfoSize = ::GetFileVersionInfoSize(szFilePath, &dwInfoSize);
		if ( dwInfoSize == 0 )
		{
			return false;
		}

		bool bRet = false;
		void *pInfo = malloc(dwInfoSize);
		if ( !::GetFileVersionInfo(szFilePath, NULL, dwInfoSize, pInfo) )
		{
			goto exit_point;
		}

		UINT uiVerLength = 0;
		void *lpVersionPtr = NULL;
		if ( !::VerQueryValue(pInfo, L"\\", &lpVersionPtr, &uiVerLength) )
		{
			goto exit_point;
		}

		VS_FIXEDFILEINFO *pFixedInfo = (VS_FIXEDFILEINFO *)lpVersionPtr;
		dwVersions[0] = HIWORD(pFixedInfo->dwFileVersionMS);
		dwVersions[1] = LOWORD(pFixedInfo->dwFileVersionMS);
		dwVersions[2] = HIWORD(pFixedInfo->dwFileVersionLS);
		dwVersions[3] = LOWORD(pFixedInfo->dwFileVersionLS);
		bRet = true;

exit_point:
		if ( NULL != pInfo )
		{
			free(pInfo);
		}

		return bRet;
	}

	bool GetRealOsVersion(LPOSVERSIONINFO lpVersionInfo)
	{
		wchar_t szBuf[MAX_PATH] = {0};
		if ( 0 == ::GetSystemDirectory(szBuf, _countof(szBuf)) )
		{
			return false;
		}

		wcscat_s(szBuf, L"\\kernel32.dll");

		WORD aVersion[4] = {0};
		if ( !GetFileVersion(szBuf, aVersion) )
		{
			return false;
		}

		lpVersionInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		lpVersionInfo->dwMajorVersion = aVersion[0];
		lpVersionInfo->dwMinorVersion = aVersion[1];
		lpVersionInfo->dwBuildNumber = aVersion[2];
		lpVersionInfo->dwPlatformId = 2; //dummy
		lpVersionInfo->szCSDVersion[0] = 0;
		
		return true;
	}

	static bool IsVista()
	{
		static bool s_bInited = false;
		static bool s_bVista = false;
		if ( !s_bInited )
		{
			s_bInited = true;
			OSVERSIONINFO ver = {0};
			if ( GetRealOsVersion(&ver) && ver.dwMajorVersion >= 6 )
			{
				s_bVista = true;
			}
		}
		
		return s_bVista;
	}

	static bool IsWin8()
	{
		static bool s_bInited = false;
		static bool s_bWin8 = false;
		if ( !s_bInited )
		{
			s_bInited = true;
			OSVERSIONINFO ver = {0};
			if ( GetRealOsVersion(&ver) && (ver.dwMajorVersion >6 || (ver.dwMajorVersion == 6 && ver.dwMinorVersion >= 2)) )
			{
				s_bWin8 = true;
			}
		}

		return s_bWin8;
	}

	static bool InnerAddAccessRight(const wchar_t *p_szName, SE_OBJECT_TYPE p_seObjType, unsigned long p_lAccessMask, bool p_bInherit)
	{
		bool bRet = false;
		DWORD dwRet = ERROR_SUCCESS;
		PSECURITY_DESCRIPTOR pSD = NULL; 
		PACL pOldDacl = NULL; 
		PACL pNewDacl = NULL; 
		EXPLICIT_ACCESSW aEA[2]; 

		if ( p_bInherit )
		{
			__try
			{
				dwRet = ::GetNamedSecurityInfoW((LPWSTR)p_szName, p_seObjType, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDacl, NULL, &pSD);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				return false;
			}
		}

		if ( dwRet != ERROR_SUCCESS )
		{
			goto exit_point; 
		}

		// 创建一个ACE，允许Everyone完全控制对象，并允许子对象继承此权限。
		::ZeroMemory(aEA, sizeof(EXPLICIT_ACCESS) * 2); 
		__try
		{
			::BuildExplicitAccessWithNameW(&aEA[0], L"Everyone", p_lAccessMask, SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT); 
			::BuildExplicitAccessWithNameW(&aEA[1], L"ALL APPLICATION PACKAGES", p_lAccessMask, SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT); 
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}

		// 将新的ACE加入DACL
		__try
		{
			int nSet = 1;
			if ( IsWin8() )
			{
				nSet = 2;
			}
			dwRet = ::SetEntriesInAclW(nSet, aEA, pOldDacl, &pNewDacl); 
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}

		if ( dwRet != ERROR_SUCCESS )
		{
			goto exit_point;
		}

		// 更新DACL 
		DWORD dwFlag = DACL_SECURITY_INFORMATION;
		if ( !p_bInherit )
		{
			dwFlag |= PROTECTED_DACL_SECURITY_INFORMATION;
		}

		__try
		{
			dwRet = ::SetNamedSecurityInfoW((LPWSTR)p_szName, p_seObjType, dwFlag, NULL, NULL, pNewDacl, NULL); 
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}

		if ( dwRet != ERROR_SUCCESS )
		{
			goto exit_point; 
		}
		bRet = true;

exit_point:
		if ( pNewDacl )
		{
			::LocalFree(pNewDacl);
		}

		if ( pSD )
		{
			::LocalFree(pSD);
		}

		return bRet;
	}

	bool AddAccessRight(const wchar_t *p_szName, SE_OBJECT_TYPE p_seObjType, unsigned long p_lAccessMask, bool p_bInherit)
	{
		bool bRet = false;

		__try
		{
			bRet = InnerAddAccessRight(p_szName, p_seObjType, p_lAccessMask, p_bInherit);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{

		}

		return bRet;
	}

	bool CreateDefaultDACL(PACL *ppDacl)
	{
		typedef struct _SID2
		{
			SID m_sid;
			DWORD m_dwRID2;
		}SID2;

		if( !ppDacl )
		{
			return false;
		}

		SID sidEveryone = {SID_REVISION, 1, SECURITY_WORLD_SID_AUTHORITY, SECURITY_WORLD_RID};
		SID sidApp0 = {SID_REVISION, 2, SECURITY_APP_PACKAGE_AUTHORITY,SECURITY_APP_PACKAGE_BASE_RID};
		SID2 sidApp = {sidApp0, SECURITY_BUILTIN_PACKAGE_ANY_PACKAGE};

		const DWORD _maxVersion2AceSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + ::GetLengthSid(&sidEveryone);
		DWORD cb = _maxVersion2AceSize * 2;
		*ppDacl = (PACL)::LocalAlloc(GPTR, cb);
		if ( !::InitializeAcl(*ppDacl, cb, ACL_REVISION) )
		{
			return false;
		}
		DWORD grfInherit = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE;
		if ( !::AddAccessAllowedAceEx(*ppDacl, ACL_REVISION, grfInherit, GENERIC_ALL|WRITE_DAC, &sidEveryone) )
		{
			return false;
		}

		if ( IsWin8() && !::AddAccessAllowedAceEx(*ppDacl, ACL_REVISION, grfInherit, GENERIC_ALL|WRITE_DAC, &sidApp) )
		{
			return false;
		}

		return true;
	}

	PACL GetDefaultDACL()
	{
		static PACL pAcl = NULL;
		static bool bFirst = true;
		if ( bFirst )
		{
			bFirst = false;
			CreateDefaultDACL(&pAcl);
		}

		return pAcl;
	}

	typedef bool (WINAPI *t_fnAddMandatoryAce)(PACL pAcl, DWORD dwAceRevision, DWORD AceFlags, DWORD MandatoryPolicy, PSID pLabelSid);

	bool CreateDefaultSACL(PACL *ppAcl)
	{
		HMODULE hMod = ::LoadLibrary(L"advapi32.dll");
		if ( NULL == hMod )
		{
			return false;
		}

		t_fnAddMandatoryAce fnAddMandatoryAce = (t_fnAddMandatoryAce)::GetProcAddress(hMod, "AddMandatoryAce");
		if ( NULL == fnAddMandatoryAce )
		{
			return false;
		}

		SID sidEveryone = {SID_REVISION, 1, SECURITY_MANDATORY_LABEL_AUTHORITY, SECURITY_MANDATORY_LOW_RID};
		const DWORD _maxVersion2AceSize = sizeof(ACL) + sizeof(SYSTEM_MANDATORY_LABEL_ACE) - sizeof(DWORD) + GetLengthSid(&sidEveryone);
		DWORD cb = _maxVersion2AceSize;
		*ppAcl = (PACL)::LocalAlloc(GPTR, cb);
		if ( !::InitializeAcl(*ppAcl, cb, ACL_REVISION) )
		{
			return false;
		}
		DWORD grfInherit = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE;
		if ( !fnAddMandatoryAce(*ppAcl, ACL_REVISION, grfInherit, 0, &sidEveryone) )
		{
			return false;
		}

		return true;
	}

	PACL GetDefaultSACL()
	{
		static PACL pAcl = NULL;
		static bool bFirst = true;
		if ( bFirst )
		{
			bFirst = false;
			CreateDefaultSACL(&pAcl);
		}

		return pAcl;
	}

	LPSECURITY_ATTRIBUTES GetDefaultSecurity()
	{
		static SECURITY_DESCRIPTOR sd = {0};
		static SECURITY_ATTRIBUTES sa = {sizeof(sa), &sd, true};
		static LPSECURITY_ATTRIBUTES psa = NULL;
		static bool bFirst = true;
		if ( bFirst )
		{
			bFirst = false;
			if ( !::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION) )
			{
				return NULL;
			}

			PACL pAcl = GetDefaultDACL();
			if ( NULL == pAcl )
			{
				return NULL;
			}
			if ( !::SetSecurityDescriptorDacl(&sd, true, pAcl, false) )
			{
				return NULL;
			}
			PACL pSAcl = GetDefaultSACL();
			if ( NULL != pSAcl )
			{
				if ( !::SetSecurityDescriptorSacl(&sd, true, pSAcl, false) )
				{
					return NULL;
				}
			}

			psa = &sa;
		}

		return psa;
	}

	bool AddAccessRight(HANDLE p_hObject, SE_OBJECT_TYPE p_seObjType, unsigned long p_lAccessMask)
	{
		static PACL pNewDacl = NULL;
		bool bRet = false;
		if ( NULL == pNewDacl && !CreateDefaultDACL(&pNewDacl) )
		{
			goto exit_point;
		}

		// 更新DACL 
		DWORD dwRet = ::SetSecurityInfo(p_hObject, p_seObjType, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDacl, NULL); 
		if ( dwRet != ERROR_SUCCESS )
		{
			goto exit_point;
		}

		bRet = true;

exit_point:
		return bRet;
	}

	bool SetLowLabel(HANDLE p_Object, SE_OBJECT_TYPE p_seObjType)
	{
		if ( !IsVista() )
		{
			return true;
		}

		DWORD dwErr = ERROR_INVALID_FUNCTION;
		PSECURITY_DESCRIPTOR pSD = NULL;    

		PACL pSacl = NULL; // not allocated
		BOOL fSaclPresent = FALSE;
		BOOL fSaclDefaulted = FALSE;

		if ( ::ConvertStringSecurityDescriptorToSecurityDescriptorW(LOW_INTEGRITY_SDDL_SACL, SDDL_REVISION_1, &pSD, NULL) )
		{
			if ( ::GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted) )
			{
				dwErr = ::SetSecurityInfo(p_Object,	p_seObjType, LABEL_SECURITY_INFORMATION, NULL, NULL, NULL, pSacl);
			}
			::LocalFree(pSD);
		}

		return (dwErr == ERROR_SUCCESS);
	}

	bool SetMediumLabel(const wchar_t *p_szName, SE_OBJECT_TYPE p_seObjType)
	{
		DWORD dwErr = ERROR_INVALID_FUNCTION;
		PSECURITY_DESCRIPTOR pSD = NULL;    

		PACL pSacl = NULL; // not allocated
		BOOL fSaclPresent = FALSE;
		BOOL fSaclDefaulted = FALSE;

		if ( ::ConvertStringSecurityDescriptorToSecurityDescriptorW(MEDIUM_INTEGRITY_SDDL_SACL, SDDL_REVISION_1, &pSD, NULL) )
		{
			if ( ::GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted) )
			{
				dwErr = ::SetNamedSecurityInfoW((LPWSTR)p_szName, p_seObjType, LABEL_SECURITY_INFORMATION, NULL, NULL, NULL, pSacl);
			}
			::LocalFree(pSD);
		}

		return (dwErr == ERROR_SUCCESS);
	}

	bool SetMediumLabel(HANDLE p_hObject, SE_OBJECT_TYPE p_seObjType)
	{
		DWORD dwErr = ERROR_INVALID_FUNCTION;
		PSECURITY_DESCRIPTOR pSD = NULL;    

		PACL pSacl = NULL; // not allocated
		BOOL fSaclPresent = FALSE;
		BOOL fSaclDefaulted = FALSE;

		if ( ::ConvertStringSecurityDescriptorToSecurityDescriptorW(MEDIUM_INTEGRITY_SDDL_SACL, SDDL_REVISION_1, &pSD, NULL) )
		{
			if ( ::GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted) )
			{
				dwErr = ::SetSecurityInfo(p_hObject, p_seObjType, LABEL_SECURITY_INFORMATION, NULL, NULL, NULL, pSacl);
			}
			::LocalFree(pSD);
		}

		return (dwErr == ERROR_SUCCESS);
	}

	bool SetLowLabel(const wchar_t *p_szName, SE_OBJECT_TYPE p_seObjType)
	{
		if( !p_szName )
		{
			return false;
		}

		if ( !IsVista() )
		{
			return true;
		}

		if ( p_seObjType == SE_FILE_OBJECT )
		{
			const wchar_t *pEnd = wcsrchr(p_szName, L'.');
			if( pEnd && _wcsicmp(pEnd, L".exe") == 0 )
			{
				return true;
			}
		}

		DWORD dwErr = ERROR_INVALID_FUNCTION;
		PSECURITY_DESCRIPTOR pSD = NULL;    

		PACL pSacl = NULL; // not allocated
		BOOL fSaclPresent = FALSE;
		BOOL fSaclDefaulted = FALSE;

		if ( ::ConvertStringSecurityDescriptorToSecurityDescriptorW(LOW_INTEGRITY_SDDL_SACL, SDDL_REVISION_1, &pSD, NULL) )
		{
			if ( ::GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted) )
			{
				dwErr = ::SetNamedSecurityInfoW((LPWSTR)p_szName,	p_seObjType, LABEL_SECURITY_INFORMATION, NULL, NULL, NULL, pSacl);
			}
			::LocalFree(pSD);
		}     

		return (dwErr == ERROR_SUCCESS);
	}

	bool SetLowLabelDir(const wchar_t *p_szName)
	{
		if ( !IsVista() )
		{
			return true;
		}

		static WIN32_FIND_DATAW fdata;//allow re-enter
		struct _stat st;
		if ( 0 != _wstat(p_szName, &st) )
		{
			return false;
		}

		if ( st.st_mode & _S_IFDIR )
		{
			wchar_t *pBuf = (wchar_t *)malloc(sizeof(wchar_t) * MAX_PATH);
			if ( NULL == pBuf )
			{
				return false;
			}

			wcscpy_s(pBuf, MAX_PATH, p_szName);
			wcscat_s(pBuf, MAX_PATH, L"\\*.*");
			HANDLE hFind = ::FindFirstFileW(pBuf, &fdata);
			if ( INVALID_HANDLE_VALUE != hFind )
			{
				do
				{
					if ( !wcscmp(fdata.cFileName, L".") || !wcscmp(fdata.cFileName, L"..") )
					{
						continue;
					}
					wcscpy_s(pBuf, MAX_PATH, p_szName);
					wcscat_s(pBuf, MAX_PATH, L"\\");
					wcscat_s(pBuf, MAX_PATH, fdata.cFileName);
					SetLowLabel(pBuf, SE_FILE_OBJECT);
				}
				while (::FindNextFileW(hFind, &fdata));
				::FindClose(hFind);
			}
			free(pBuf);
		}

		SetLowLabel(p_szName, SE_FILE_OBJECT);
		return true;
	}
}
