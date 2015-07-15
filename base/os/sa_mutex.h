#ifndef _SA_MUTEX_H_
#define _SA_MUTEX_H_

#include "sa_config.h"

#if defined(SA_WIN)

#include "windows/mutex_win.h"

#elif defined(SA_LINUX)

#include "linux/mutex_linux.h"

#else

#endif
#include <assert.h>

template <class T>
class t_autoLocker 
{
public:
	t_autoLocker(T &p_mutex, bool p_bLock = true ) 
	:m_mutex(p_mutex), m_bLocked(false)
	{
		if ( p_bLock == true )
		{
			Lock();
		}
	}
	~t_autoLocker()
	{
		Unlock();
	}
	bool Lock()
	{
		assert( m_mutex.IsOpen() );
		bool bRet = m_mutex.Lock();
		if ( bRet == false )
		{
			assert(false);
			return false;
		}
		m_bLocked = true;
		return true;
	}

	bool Unlock()
	{
		if ( m_bLocked == true )
		{
			m_mutex.Unlock();
			m_bLocked = false;
		}
		return true;
	}
	
protected:
	T &m_mutex;
	bool m_bLocked;
};
#endif
