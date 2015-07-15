#ifndef _SA_FILE_H_
#define _SA_FILE_H_

#include "sa_config.h"

#if defined(SA_WIN)

#include "windows/file_win.h"

#elif defined (SA_LINUX)

#include "linux/file_linux.h"

#else

#endif

#endif
