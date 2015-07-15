#include "thread_linux.h"
#include <assert.h>

t_thread::t_thread()
{
	m_iOpened = 0;
}

t_thread::~t_thread()
{
	Close();
}

bool t_thread::Open()
{
	int iRet = pthread_attr_init(&m_thrAttr);
	if( iRet < 0 )
	{
		return false;
	}
	return true;
}

bool t_thread::Close()
{
	if( m_iOpened == 1 )
	{
		pthread_join(m_thrID, NULL);
		pthread_attr_destroy(&m_thrAttr);
		m_iOpened = 0;
	}
	return true;
}

bool t_thread::Start(thread_run_t p_pThreadFunc, void *p_pArg)
{
	int iRet = pthread_create(&m_thrID, &m_thrAttr, p_pThreadFunc, p_pArg);
	if( iRet != 0 )
	{
		assert(false);
		return false;
	}
	m_iOpened = 1;
	return true;
}

bool t_thread::Cancel()
{
	int iRet = pthread_cancel(m_thrID);
	if ( iRet != 0 )
	{
		assert(false);
		return false;
	}
	return true;
}

bool t_thread::TestCancel()
{
	pthread_testcancel();
	return false;
}