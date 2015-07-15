#ifndef _OS_LINUX_H_
#define _OS_LINUX_H_

namespace os
{
	const static unsigned int MAX_PATH_LEN = 1024;
	int ListDir(const wchar_t *p_pDirName, wchar_t p_pFilenames[][MAX_PATH_LEN], int p_iMaxFilenames);
	bool Remove(const wchar_t *p_pFileName);

};

#endif
