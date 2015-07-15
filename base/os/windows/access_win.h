// author: helitao
#ifndef _ACCESS_WIN_H_
#define _ACCESS_WIN_H_

#include <Aclapi.h>

namespace n_access
{
	bool AddAccessRight(const wchar_t *p_szName, SE_OBJECT_TYPE p_seObjType, unsigned long p_lAccessMask, bool p_bInherit);
	bool AddAccessRight(HANDLE p_hObject, SE_OBJECT_TYPE p_seObjType, unsigned long p_lAccessMask);

	bool SetLowLabel(const wchar_t *p_szName, SE_OBJECT_TYPE p_seObjType);
	bool SetLowLabel(HANDLE p_hObject, SE_OBJECT_TYPE p_seObjType);
	bool SetMediumLabel(const wchar_t *p_szName, SE_OBJECT_TYPE p_seObjType);
	bool SetMediumLabel(HANDLE p_hObject, SE_OBJECT_TYPE p_seObjType);

	bool SetLowLabelDir(const wchar_t *p_szName); //for dir

	LPSECURITY_ATTRIBUTES GetDefaultSecurity();
}

#endif	// _ACCESS_WIN_H_