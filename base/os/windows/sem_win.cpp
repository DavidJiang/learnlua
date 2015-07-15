#include "sem_win.h"
#include <assert.h>

bool t_sem::Open(unsigned int p_iInitialCount)
{
	m_semID = ::CreateSemaphore(NULL,p_iInitialCount, MAX_SEM_COUNT, NULL);
	if( m_semID == NULL )
	{
		assert(false);
		return false;
	}
	return true;
}

t_sem::~t_sem()
{
	Close();
}

bool t_sem::Close()
{
	if ( m_iOpened == 1 )
	{
		::CloseHandle(m_semID);
		m_semID = NULL;
		m_iOpened = 0;
	}
	return true;
}

bool t_sem::Post()
{
	BOOL bRet = ::ReleaseSemaphore(m_semID, 1, NULL);
	if ( bRet == FALSE )
	{
		assert(false);
		return false;
	}
	return true;
}

bool t_sem::Wait(unsigned int p_iMilliseconds)
{
	DWORD dwRet = ::WaitForSingleObject(m_semID, p_iMilliseconds);
	if ( dwRet == WAIT_TIMEOUT )
	{
		return false;
	}
	return true;
}

