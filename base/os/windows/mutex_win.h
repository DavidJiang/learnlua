#ifndef _MUTEX_WIN_H_
#define _MUTEX_WIN_H_

#include <Windows.h>
//todo: 
//#include <Synchapi.h> on Windows 8 and Windows Server 2012

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
	CRITICAL_SECTION m_cs;
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
	bool IsOpen() const{ return m_bOpened; }
	bool Lock();
	bool Unlock();
protected:
	HANDLE m_hMutex;
	bool m_bOpened;
	wchar_t *m_szLastError;
};

#endif
