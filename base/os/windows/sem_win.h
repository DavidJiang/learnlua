#ifndef _SEM_WIN_H_
#define _SEM_WIN_H_

#include <windows.h>

class t_sem
{
public:
	t_sem():m_iOpened(0){}
	~t_sem();
	const static unsigned int MAX_SEM_COUNT = 100;
	bool Open(unsigned int p_iInitialCount);
	bool Close();
	bool Post();
	bool Wait(unsigned int p_iMilliseconds = INFINITE );
protected:
	HANDLE m_semID;
	int m_iOpened;
};
#endif