// author: helitao
#ifndef _FILEMAP_WIN_H_
#define _FILEMAP_WIN_H_

#include <Windows.h>
#include "string_win.h"
#include "path_win.h"

class t_filemap
{
public:
	typedef enum _eTagMapState
	{
		ec_uninitiated, 
		ec_mapped,		/**< file opened and mapped. */
		ec_unmapped,	/**< file unmapped but not closed. */
	}t_mapState;

	t_filemap();
	~t_filemap();

	// 基于文件的映射（文件只读）
	bool Open(const wchar_t *p_szFilePath, const wchar_t *p_pShmname);
	bool Open(const std::wstring &p_strFilePath, const std::wstring &p_strShmName);
	bool Open(const t_path &p_path, const std::wstring &p_strShmName);

	// 基于内存的，可读写的映射
	bool OpenNC(const wchar_t *p_szName);//open 如果不存在的话不进行创建
	bool Open(const wchar_t *p_szName, int p_nSize);
	bool Open(const std::wstring &p_strName, int p_nSize);
	bool IsOpen();
	bool IsCreator();
	bool Close();

	int  GetSize();
	unsigned char *GetDataPtr();

	const wchar_t *GetLastError();
	void *BaseAddr(void) const { return m_pData; }
	unsigned int Size(void) const { return m_nSize; }
	bool Sync(void *p_pAddr = NULL, size_t p_length = 0);

private:
	bool MakeName(const wchar_t *p_szOriginal, const wchar_t *p_szPrefix, const wchar_t *p_szSuffix, std::wstring &p_strName);

private:
	wchar_t *m_szLastError;
	std::wstring m_strName;
	HANDLE   m_hFilemap;
	int		 m_nSize;
	unsigned char *m_pData;
	bool	 m_bCreator;
	t_mapState m_state;
};

#endif	// _FILEMAP_WIN_H_