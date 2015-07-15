#ifndef _SA_OS_H_
#define _SA_OS_H_

#include "sa_config.h"

#if defined(SA_WIN)

#include "./windows/os_win.h"

#elif defined (SA_LINUX)

#include "./linux/os_linux.h"

#else

#endif


#endif
