#ifndef _FILE_MAP_LINUX_H_
#define _FILE_MAP_LINUX_H_

#include <wchar.h>

#include "path_linux.h"

#ifndef MAX_PATH_LEN 
#define MAX_PATH_LEN 1024
#endif

class t_filemap
{
public:
	typedef enum _eTagMapState
	{
		ec_uninitiated, 
		ec_mapped, /**< file opened and mapped. */
		ec_unmapped, /**< file unmapped but not closed. */
	}t_mapState;
	t_filemap();
	t_filemap( const wchar_t *p_pPathName, const wchar_t *p_pShmName = NULL);
	t_filemap( const t_path & p_path, const wchar_t *p_pShmName = NULL);
	
	
	~t_filemap();
	bool Open(const wchar_t *p_pPathName, const wchar_t *p_pShmName = NULL);
	bool Open(const t_path & p_path, const wchar_t *p_pShmName = NULL);

	/**
	 *@brief used for shared memory creation.
	 */
	bool Open(const wchar_t *p_pShmName, int p_iShmSize);
	bool OpenNC(const wchar_t *p_pShmName);//open 如果不存在的话不进行创建
	
	bool IsOpen();
	bool IsCreator(){return m_bIsCreator;}
	bool Close();
	bool Sync(void *p_pAddr = NULL , size_t p_length = 0);
	void *BaseAddr(void) const { return m_pAddr; }
	unsigned int Size(void) const { return m_iSize; }
	unsigned char *GetDataPtr(){ return (unsigned char *)m_pAddr; }
	unsigned int GetSize(){ return m_iSize; }
	const wchar_t *GetLastError(){ return L"no-error"; }

protected:
	void *m_pAddr;
	size_t m_iSize;
	t_mapState m_state;
	int m_iFileDesc;
	bool m_bIsCreator; /**< stands for new created or just opened. */
	char m_szShmName[MAX_PATH_LEN];
};

#endif
