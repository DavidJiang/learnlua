#ifndef _SA_WCHAR_H_
#define _SA_WCHAR_H_

#include "sa_config.h"

#if defined (SA_WIN)

#include <wchar.h>

#elif defined (SA_LINUX)

#include <wchar.h>

#define _tmemcmp	wmemcmp_emulation
#define wcsnicmp   wcsncasecmp
#define _wcsicmp    wcscasecmp
#define _wtoi(str)		wcstol(str, 0, 10)
#define wcsncpy_s(dest, destlen, source, count)	wcsncpy(dest, source, count)
#define swprintf_s swprintf

#elif defined (SA_MAC)

#else

#endif

#endif // _SA_WCHAR_H_
