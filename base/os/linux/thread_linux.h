#ifndef _THREAD_LINUX_H_
#define _THREAD_LINUX_H_

typedef void * thread_value_t;
typedef void *(*thread_run_t)(void *);

#define __stdcall 

#include <pthread.h>
class t_thread
{
public:
	t_thread();
	~t_thread();

	bool Open();
	bool Close();

	virtual bool Start(thread_run_t p_pThreadFunc, void *p_pArg);
	bool Cancel();
	bool TestCancel();

protected:
	pthread_t m_thrID;
	pthread_attr_t m_thrAttr;
	int m_iOpened;
};
#endif