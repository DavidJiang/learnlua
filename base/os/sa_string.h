#ifndef _SA_STRING_H_
#define _SA_STRING_H_

#include "sa_config.h"

#if defined (SA_WIN)
#include "windows/string_win.h"

#elif defined (SA_LINUX)
//#include "linux/wstring_linux.h"
#include <string.h>

#elif defined (SA_MAC)

#else

#endif	


#endif // _SA_STRING_H_
