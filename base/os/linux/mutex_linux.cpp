#include "mutex_linux.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "strconvert_linux.h"

#include <sys/mman.h>
#include <sys/stat.h>     
#include <fcntl.h>        
#include <unistd.h>
#include <assert.h>

t_threadMutex::t_threadMutex():m_bOpened(false)
{
	
}

t_threadMutex::~t_threadMutex()
{
	Close();
}

bool t_threadMutex::Open()
{
	pthread_mutexattr_t attr;
	int ret = pthread_mutexattr_init(&attr);
	if( ret != 0 )
	{
		perror( strerror( errno) );
		return false;
	}

	/**< set to recursive lock. */
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

	ret = pthread_mutex_init(&m_mutex, &attr);
	if( ret != 0 )
	{
		perror( strerror(errno) );
		return false;
	}
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
		int ret = pthread_mutex_destroy(&m_mutex);
		if( ret != 0 )
		{
			perror( strerror(errno) );
			return false;
		}
		m_bOpened = false;
	}
	return true;
}

bool t_threadMutex::Lock()
{
	int ret = pthread_mutex_lock(&m_mutex);
	if( ret != 0 )
	{
		perror( strerror(errno) );
		return false;
	}
	return true;
}

bool t_threadMutex::Unlock()
{
	int ret = pthread_mutex_unlock(&m_mutex);
	if( ret != 0 )
	{
		perror( strerror( errno) );
		return false;
	}
	return true;
}

t_processMutex::t_processMutex():
	m_pMutex(NULL), m_bOpened(false)
{
	
}

t_processMutex::~t_processMutex()
{
	Close();
}

bool t_processMutex::Open(const wchar_t *p_szName)
{
	//todo: check _POSIX_SHARED_MEMORY_OBJECTS
	//sysconf(_SC_THREAD_PROCESS_SHARED)
	memset(m_szMutexName, 0, MAX_MUTEX_NAME);
	int iActualLen = MAX_MUTEX_NAME;
	bool bRet = t_strConverter::W2C(p_szName, m_szMutexName, iActualLen);
	if( bRet == false )
	{
		return false;
	}
	mode_t oldmode = umask(0);
	int fd  = shm_open(m_szMutexName, O_RDWR|O_CREAT, 0666);
	if( fd == -1 )
	{
		assert(false);
		perror( strerror(errno) );
		umask(oldmode);
		return false;
	}
	umask(oldmode);
	ftruncate(fd, sizeof(pthread_mutex_t) );
	m_pMutex =(pthread_mutex_t*)mmap(NULL,sizeof(pthread_mutex_t), 
									 PROT_READ|PROT_WRITE, MAP_SHARED, 
									 fd, 0);
	if( m_pMutex == MAP_FAILED )
	{
		assert(false);
		perror(strerror( errno) );
		close(fd);
		return false;
	}
	close(fd);

	pthread_mutexattr_init(&m_mutexAttr);
	int ret = pthread_mutexattr_setpshared(&m_mutexAttr, PTHREAD_PROCESS_SHARED);
	if( ret != 0 )
	{
		assert(false);
		perror( strerror(errno) );
		return false;
	}
	ret = pthread_mutex_init(m_pMutex, &m_mutexAttr);
	if( ret != 0 )
	{
		assert(false);
		perror( strerror(errno) );
		return false;
	}
	
	m_bOpened = true;
	return true;
}

bool t_processMutex::Close()
{
	int ret = 0;
	if( m_bOpened == true )
	{
		ret = pthread_mutex_destroy(m_pMutex);
		if( ret != 0 )
		{
			assert(false);
			perror( strerror(errno) );
		}
		m_pMutex = NULL;
        ret = pthread_mutexattr_destroy(&m_mutexAttr);
        if ( ret != 0 )
        {
        	assert(false);
            perror( strerror(errno) );
            return false;
        }
		if( m_pMutex != NULL )
		{
			munmap(m_pMutex, sizeof(pthread_mutex_t) );
		}
		shm_unlink(m_szMutexName);
		m_bOpened = false;
	}

	return true;
}

bool t_processMutex::Lock()
{
	int ret = pthread_mutex_lock(m_pMutex);
	if( ret != 0 )
	{
		perror( strerror(errno) );
		return false;
	}
	return true;
}

bool t_processMutex::Unlock()
{
	int ret = pthread_mutex_unlock(m_pMutex);
	if( ret != 0 )
	{
		perror( strerror(errno) );
		return false;
	}
	return true;
}


