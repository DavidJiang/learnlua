#ifndef _SA_STR_CONVERT_H_
#define _SA_STR_CONVERT_H_

#include "sa_config.h"

#if defined(SA_WIN)

#include "windows/strconvert_win.h"

#elif defined (SA_LINUX)

#include "linux/strconvert_linux.h"


#endif

#endif
