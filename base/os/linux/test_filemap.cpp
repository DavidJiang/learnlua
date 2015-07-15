#include "filemap_linux.h"

#include <string.h>
#include <stdio.h>

int main( int argc, char *argv[] )
{
	t_filemap fmap;
	bool bRet = fmap.Open(L"haha", 1024 * 1024 * 200);
	if( bRet == false )
	{
		return -1;
	}
	void *addr = fmap.BaseAddr();
	int iSize = fmap.Size();
	char *psrc = "wolegequ";
	memcpy((char *)addr, psrc, strlen(psrc) );

	getchar();
	return 0;
}
