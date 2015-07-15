#ifndef _MUTEX_LINUX_H_
#define _MUTEX_LINUX_H_

#include <pthread.h>

class t_threadMutex
{
public:
	t_threadMutex();
	~t_threadMutex();
	bool Open();
	/**
	 *@brief this is just for the interface same with 
	 * process mutex.
	 */
	bool Open(const wchar_t *p_szName);
	bool IsOpen(){ return m_bOpened; }
	bool Close();
	bool Lock();
	bool Unlock();

protected:
	pthread_mutex_t m_mutex;
	bool m_bOpened;
	
};

class t_processMutex
{
public:
	const static unsigned int MAX_MUTEX_NAME = 1024;
	t_processMutex();
	~t_processMutex();
	bool Open(const wchar_t *p_szName);
	bool Close();
	bool IsOpen(){ return m_bOpened; }
	bool Lock();
	bool Unlock();
protected:
	pthread_mutex_t *m_pMutex;
	pthread_mutexattr_t m_mutexAttr;
	bool m_bOpened;
	char m_szMutexName[MAX_MUTEX_NAME];
};

#endif
