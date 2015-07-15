#ifndef _THREAD_WIN_H_
#define _THREAD_WIN_H_

#include <windows.h>

typedef unsigned thread_value_t;
typedef unsigned (__stdcall *thread_run_t)(void *);

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
	HANDLE m_thrdID;
	int m_iOpened;
};

#endif