#include "sem_linux.h"
#include <assert.h>
#include <time.h>
#include <errno.h>

bool t_sem::Open(unsigned int p_iInitialCount)
{
	int iRet = sem_init(&m_semID, 0, p_iInitialCount);
	if ( iRet == -1 )
	{
		assert(false);
		return false;
	}
	m_iOpened = 1;
	return true;
}

bool t_sem::Close()
{
	if ( m_iOpened == 1 )
	{
		int iRet = sem_destroy(&m_semID);
		if( iRet == -1 )
		{
			assert(false);
			return false;
		}
		m_iOpened = 0;	
	}
	return true;
}

bool t_sem::Post()
{
	int iRet = sem_post(&m_semID);
	if ( iRet == -1 )
	{
		assert(false);
		return false;
	}
	return true;
}

bool t_sem::Wait(unsigned int p_iMilliseconds)
{
	int iRet = 0;
	if ( p_iMilliseconds == INFINITE )
	{
		iRet = sem_wait(&m_semID);
		if( errno == ETIMEDOUT )
		{
			return false;				
		}
		else
		{
			assert(false);
			return false;
		}

		return true;
	}
	else
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		int iSeconds = p_iMilliseconds / 1000;
		ts.tv_sec += iSeconds;
		ts.tv_nsec += (p_iMilliseconds - iSeconds * 1000) * 1000;
		iRet = sem_timedwait(&m_semID, &ts);
		if ( iRet == -1 )
		{
			if( errno == ETIMEDOUT )
			{
				return false;				
			}
			else
			{
				assert(false);
				return false;
			}
		} 
		return true;
	}
}

