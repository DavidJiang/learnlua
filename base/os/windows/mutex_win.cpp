#include "mutex_win.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "access_win.h"

t_threadMutex::t_threadMutex():m_bOpened(false)
{
	
}

t_threadMutex::~t_threadMutex()
{
	Close();
}

bool t_threadMutex::Open()
{
	InitializeCriticalSection(&m_cs);

	m_bOpened = true;
	return true;
}


bool t_threadMutex::Open(const wchar_t *p_szName)
{
	return Open();
}


bool t_threadMutex::Close()
{
	if( m_bOpened == true )
	{
		DeleteCriticalSection(&m_cs);
		m_bOpened = false;
	}
	return true;
}

bool t_threadMutex::Lock()
{
	EnterCriticalSection(&m_cs);
	return true;
}

bool t_threadMutex::Unlock()
{
	LeaveCriticalSection(&m_cs);
	return true;
}

t_processMutex::t_processMutex():
	m_hMutex(NULL), m_bOpened(false)
{
	
}

t_processMutex::~t_processMutex()
{
	Close();
}

bool t_processMutex::Open(const wchar_t *p_szName)
{
	m_hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, p_szName);
	if( m_hMutex )
	{
		return true;
	}

	m_hMutex = ::CreateMutex(n_access::GetDefaultSecurity(), FALSE, p_szName);
	if( !m_hMutex )
	{
		return false;
	}

	n_access::AddAccessRight(m_hMutex, SE_KERNEL_OBJECT, GENERIC_ALL);
	n_access::SetLowLabel(m_hMutex, SE_KERNEL_OBJECT);

	m_bOpened = true;
	return true;
}

bool t_processMutex::Close()
{
	if( m_hMutex )
	{
		::CloseHandle(m_hMutex);
		m_hMutex = 0;
	}

	m_szLastError = 0;
	return true;
}

bool t_processMutex::Lock()
{
	if( !m_hMutex )
	{
		m_szLastError = L"加锁失败：尚未初始化";
		return false;
	}

	m_szLastError = 0;
	DWORD dwRes = ::WaitForSingleObject(m_hMutex, INFINITE);
	switch(dwRes)
	{
	case WAIT_OBJECT_0:
		break;
	case WAIT_FAILED:
		m_szLastError = L"加锁失败：锁已失效";
		return false;
	case WAIT_ABANDONED: 
		break;
	case WAIT_TIMEOUT:
		m_szLastError = L"加锁失败：超时";
		return false;
	default:
		m_szLastError = L"加锁失败：未知错误";
		assert(false);
		return false;
	}
	return true;
}

bool t_processMutex::Unlock()
{
	if( !m_hMutex )
	{
		m_szLastError = L"尚未初始化";
		return false;
	}

	if( !::ReleaseMutex(m_hMutex) )
	{
		m_szLastError = L"解锁失败：未知错误";
		return false;
	}

	m_szLastError = 0;
	return true;
}


