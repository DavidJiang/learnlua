#ifndef _SA_PROFILE_H_
#define _SA_PROFILE_H_
#include "os/sa_config.h"

#if defined (SA_WIN)

#include "windows/profile_win.h"

#elif defined (SA_LINUX)

#include "linux/profile_linux.h"

#else

#endif

#endif
