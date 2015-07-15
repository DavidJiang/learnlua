#ifndef _FILE_LINUX_H_
#define _FILE_LINUX_H_

#include "path_linux.h"
#include <vector>
#include <string>

class t_file
{
public:
	friend class t_fileUtil;

	typedef enum _eTagOpenMode
	{
		ec_invalid = 0,
		ec_in = 0x01, /**< open for reading. */
		ec_out = 0x02, /**< open for writing. */
		ec_app = 0x04, /**< seek to end of file when open. */
		ec_trunc = 0x08, /**< discarding file content when open. */
	}t_openMode;
	enum _eTagWhence
	{
		ec_seekSet,
		ec_seekCur,
		ec_seekEnd,
	};
	typedef enum _eFileState
	{
		ec_uninitiated,
		ec_opened,
		ec_closed,
	}t_fileState;

	t_file();
	explicit t_file(const wchar_t *p_pPathName, int p_mode = ec_out);
	explicit t_file(const t_path & p_path, int p_mode = ec_out);
	bool IsOpen(void);
	bool Open(const wchar_t *p_pPathName, int p_mode = ec_out);
	bool Open(const t_path & p_path, int  p_mode = ec_out);

	bool Close(void);
	bool Read(unsigned char *p_buf, int p_iReadCount, int & p_iActualCount);
	bool Write(const unsigned char *p_buf, int p_iWriteCount, int &p_iActualCount);
	bool Flush(void);
	bool Seek(unsigned int p_pos, int p_iWhence = ec_seekSet);
	unsigned int  GetCurPos(void);
	int GetSize();

	const wchar_t *GetLastError(){ return L"no-error"; }

	~t_file();
protected:
	void TranslateMode2ActualFlags(int p_mode, int &p_iFlags, int &p_iMode);
	
protected:
	const wchar_t *m_szLastError;
	t_path m_filePath; 
	int m_iFileDesc;
	int  m_openMode;
	t_fileState m_state;
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

#endif
