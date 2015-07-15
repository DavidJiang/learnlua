#ifndef _SA_PATH_H_
#define _SA_PATH_H_

#include "sa_config.h"

#if defined (SA_WIN)

#include "windows/path_win.h"

#elif defined (SA_LINUX)

#include "linux/path_linux.h"

#elif defined (SA_MAC)

#else

#endif

#endif // _SA_PATH_H_