#ifndef _SEM_LINUX_H_
#define _SEM_LINUX_H_

#include <semaphore.h>
class t_sem
{
public:
	t_sem() : m_iOpened(0){}
	~t_sem() { Close(); }
	const static unsigned int INFINITE = -1;
	bool Open(unsigned int p_iInitialCount);
	bool Close();
	bool Post();
	bool Wait(unsigned int p_iMilliseconds = INFINITE );
protected:
	sem_t m_semID;
	int m_iOpened;
};
#endif