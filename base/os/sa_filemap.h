#ifndef _SA_FILE_MAP_H_
#define _SA_FILE_MAP_H_

#include "sa_config.h"

#if defined(SA_WIN)

#include "windows/filemap_win.h"

#elif defined(SA_LINUX)

#include "linux/filemap_linux.h"

#else

#endif

#endif
