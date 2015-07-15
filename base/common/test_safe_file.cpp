#include "sa_safe_file.h"
#include "sa_mutex.h"

#include <stdio.h>
#include "sa_config.h"

//int main(int argc, char *argv[])
//{
//#if defined(SA_LINUX)
//	const wchar_t *pKeyPath = L"/home/shawn/work/docs/chenxiaowei/key.txt";
//#elif defined(SA_WIN)
//	const wchar_t *pKeyPath = L"E:\\work\\ImeKernel\\trunk\\common\\windows\\key.txt";
//#endif
//
//	t_safeFile<t_threadMutex> tsFile(pKeyPath );
//	if( tsFile.IsOpen() == false )
//	{
//		assert(false);
//	}
//	
//	tsFile.Close();
//
//	t_safeFile<t_processMutex> psFile(pKeyPath);
//	if( psFile.IsOpen() == false )
//	{
//		assert(false);
//	}
//	psFile.Close();
//
//	getchar();
//	return 0;
//}
