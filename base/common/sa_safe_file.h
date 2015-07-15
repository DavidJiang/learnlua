#ifndef _SA_SAFE_FILE_H_
#define _SA_SAFE_FILE_H_

#include "sa_basicTypes.h"
#include "os/sa_file.h"
#include "os/sa_path.h"

template<class LOCK>
class t_safeFile
{
public:
	t_safeFile();
	explicit t_safeFile(const wchar_t *p_pPathName, 
						int p_mode = t_file::ec_out);

	explicit t_safeFile(const t_path &p_path, 
						int p_mode = t_file::ec_out );
	bool IsOpen(void);
	bool Open(const wchar_t *p_pPathName, int p_mode = t_file::ec_out);
	bool Open(const t_path &p_path, int p_mode = t_file::ec_out);

	bool Close(void);
	bool Read(byte *p_buf, int p_iReadCount, int &p_iActualCount);
	bool Write(byte *p_buf, int p_iWriteCount, int &p_iActualCount);
	bool Flush(void);
	bool Seek(uint p_pos, int p_iWhence = t_file::ec_seekSet);
	uint GetCurPos(void);
	int  GetSize();
	~t_safeFile();
public:
	bool InitLock(const wchar_t *p_pPathName);
	bool UnInitLock();
protected:
	LOCK m_locker;
	t_file m_file;
	bool m_bState;
};

#include "sa_safe_file.inl"

#include "os/sa_mutex.h"

typedef t_safeFile<t_threadMutex> t_threadSafeFile;
typedef t_safeFile<t_processMutex> t_processSafeFile;

#endif
