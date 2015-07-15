#ifndef _SA_THREAD_H_
#define _SA_THREAD_H_

#include "sa_config.h"

#if defined(SA_WIN)

#include "windows/thread_win.h"

#elif defined(SA_LINUX)

#include "linux/thread_linux.h"

#else
//todo:
#endif

#endif
