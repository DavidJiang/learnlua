#ifndef _OS_CONFIG_H_
#define _OS_CONFIG_H_

#if defined (_WIN32) || defined (_WIN64)
#define SA_WIN
#include <Windows.h>

#elif defined (__linux)
#define SA_LINUX

#elif defined (__MAC)
#define SA_MAC

#else

#endif

#endif // _OS_CONFIG_H_
