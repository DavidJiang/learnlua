#ifndef _SA_SEMAPHORE_H_
#define _SA_SEMAPHORE_H_

#include "sa_config.h"

#if defined(SA_WIN)

#include "windows/sem_win.h"

#elif defined(SA_LINUX)

#include "linux/sem_linux.h"

#else
//todo:
#endif

#endif