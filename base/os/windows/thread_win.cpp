#include "thread_win.h"
#include <process.h>
#include <assert.h>

t_thread::t_thread()
{
	m_iOpened = 0;
	m_thrdID = NULL;
}

t_thread::~t_thread()
{
	Close();
}

bool t_thread::Open()
{
	return true;
}

VOID CALLBACK APCProc( ULONG_PTR dwParam )
{
}

bool t_thread::Close()
{
	if( m_iOpened == 1 )
	{
		DWORD dwRet = ::WaitForSingleObject(m_thrdID, 1000);
		if( dwRet == WAIT_TIMEOUT )
		{
			::TerminateThread(m_thrdID, 0);	
		}

		::CloseHandle(m_thrdID);
		m_thrdID = NULL;
	}
	return true;
}

bool t_thread::Start(thread_run_t p_pThreadFunc, void *p_pArg)
{
	m_thrdID = (HANDLE)::_beginthreadex(NULL, 0, p_pThreadFunc, p_pArg, 0, NULL);
	if( m_thrdID == 0 )
	{
		assert(false);
		return false;
	}
	m_iOpened = 1;
	return true;
}

bool t_thread::Cancel()
{
	DWORD dwRet = ::QueueUserAPC(APCProc, m_thrdID, NULL);
	if ( dwRet == 0 )
	{
		assert(false);
		return false;
	}
	return true;
}

bool t_thread::TestCancel()
{
	DWORD dwRet = ::SleepEx(0, TRUE);
	if( dwRet == WAIT_IO_COMPLETION )
	{
		return true;
	}
	return false;
}