// author: helitao
#ifndef _FILE_WIN_H_
#define _FILE_WIN_H_

#include <Windows.h>
#include <vector>
#include "string_win.h"
#include "path_win.h"

class t_file
{
public:
	enum e_openMode
	{
		ec_invalid = 0,
		ec_in = 0x01, /**< open for reading. */
		ec_out = 0x02, /**< open for writing. */
		ec_app = 0x04, /**< seek to end of file when open. */
		ec_trunc = 0x08, /**< discarding file content when open. */
	};

	enum e_whenceMode
	{
		ec_seekSet = 0,
		ec_seekCur,
		ec_seekEnd,
	};

	typedef struct _openParam
	{
		unsigned int	m_cOpenMode;
		unsigned long	m_dwCreate;
		unsigned long	m_dwShare;
		unsigned long	m_dwAccess;
	}t_openParam;

	t_file();
	t_file(const wchar_t *p_fileName, int p_mode);
	t_file(const std::wstring &p_fileName, int p_mode);
	t_file(const t_path &p_fileName, int p_mode = ec_out);
	~t_file();

	bool Open(const wchar_t *p_fileName, int p_mode);
	bool Open(const std::wstring &p_fileName, int p_mode);
	bool Open(const t_path &p_fileName, int p_mode);
	bool Close();

	bool Read(unsigned char *p_szBuf, int p_nSize, int &p_nRead);
	bool Write(const unsigned char *p_szBuf, int p_nSize, int &p_nWritten);
	bool Flush();
	bool Seek(int p_nPos, e_whenceMode p_origin);
	int  GetCurPos();
	bool IsOpen();
	int  GetSize();
	
	const wchar_t *GetLastError();
	const t_path &GetPath();

private:
	t_openParam *GetOpenParam(int p_mode);

protected:
	const wchar_t *m_szLastError;
	t_path m_strPath;
	HANDLE m_hFile;
	int  m_openMode;
	static t_openParam s_openParams[];
};

class t_fileUtil
{
public:
	static  bool FileExists(const t_path &p_path);
	static	bool FolderExists(const t_path &p_path);
	static	bool GetFileSize(const t_path &p_path, size_t &p_fileSize);
	static	bool RemoveFile(const t_path &p_path);
	static	bool MakeDir(const t_path &p_path);
	static  bool RemoveDir(const t_path &p_path);
	static	bool CopyFile(const t_path &p_fromPath, const t_path &p_toPath);
	static	bool MoveFile(const t_path &p_fromPath, const t_path &p_toPath);

	static	bool GetTempPath(t_path &p_pathTemp);
};

class t_fileEnum
{
public:
	t_fileEnum(const wchar_t *p_szDir, bool p_bRecursive, const wchar_t *p_szFilter = 0);
	~t_fileEnum();

	int GetFileCount();
	const wchar_t *GetFileName(int p_nIndex);

private:
	void EnumFiles(const wchar_t *p_szDir, bool p_bRecursive, const wchar_t *p_szFilter);

private:
	std::vector<std::wstring> m_vFiles;
};

#endif	// _FILE_WIN_H_