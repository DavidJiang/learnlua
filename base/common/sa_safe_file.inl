#include "sa_safe_file.h"
#include "sa_utility.h"
#include "sa_assert.h"
#include <string>
template<class LOCK>
t_safeFile<LOCK>::t_safeFile()
{}

template<class LOCK>
inline t_safeFile<LOCK>::t_safeFile(const wchar_t *p_pPathName, 
	   int p_mode ):
	m_file(p_pPathName, p_mode)
{
	m_bState = InitLock(p_pPathName);
}

template<class LOCK>
inline t_safeFile<LOCK>::t_safeFile(const t_path &p_path, 
	   int p_mode):
	m_file(p_path, p_mode)
{
	m_bState = InitLock(p_path.FullPath().c_str() );
}

template<class LOCK>
inline bool t_safeFile<LOCK>::InitLock(const wchar_t *p_pPathName)
{
	std::wstring strLockerName;
	bool bRet = n_utility::MakeName(p_pPathName, L"File_Locker", 
								   NULL, strLockerName);
	if( bRet == false )
	{
		SA_ASSERT_RETURN(__FILE__, __LINE__, 
						 "utility makename failed.", false, false);
	}
	bRet = m_locker.Open( strLockerName.c_str() );
	if( bRet == false )
	{
		SA_ASSERT_RETURN(__FILE__, __LINE__,
						 "create file lock failed.", false, false);
	}
	return m_locker.Lock();
}

template<class LOCK>
inline bool t_safeFile<LOCK>::IsOpen(void)
{
	if( m_bState == true && m_file.IsOpen() == true )
	{
		return true;
	}
	return false;
}

template<class LOCK>
inline bool t_safeFile<LOCK>::Open(const wchar_t *p_pPathName, int p_mode)
{
	bool bRet = m_file.Open(p_pPathName, p_mode);
	if( bRet == false )
	{
		SA_ASSERT_RETURN(__FILE__, __LINE__,
						 "open file failed.", false, false);
	}
	bRet = InitLock(p_pPathName);
	if( bRet == false )
	{
		SA_ASSERT_RETURN(__FILE__, __LINE__, 
						 "initialize file lock failed", false, false);
	}
	return true;
}

template<class LOCK>
inline bool t_safeFile<LOCK>::Open(const t_path &p_path, int p_mode)
{
	return Open(p_path.FullPath().c_str(), p_mode);
}

template<class LOCK>
inline 	int t_safeFile<LOCK>::GetSize()
{
	return m_file.GetSize();
}

template<class LOCK>
inline bool t_safeFile<LOCK>::Close(void)
{
	bool bRet = m_file.Close();
	if( bRet == false )
	{
		SA_ASSERT_RETURN(__FILE__, __LINE__, 
						 "close file failed.", false, false);
	}
	bRet = UnInitLock();
	if( bRet == false )
	{
		SA_ASSERT_RETURN(__FILE__, __LINE__,
						 "close lock failed.", false, false);
	}
	return true;
}

template<class LOCK>
inline bool t_safeFile<LOCK>::UnInitLock()
{
	m_locker.Unlock();
	m_locker.Close();
	return true;
}

template<class LOCK>
inline bool t_safeFile<LOCK>::Read(byte *p_buf, int p_iReadCount, 
	 int &p_iActualCount)
{
	return m_file.Read(p_buf, p_iReadCount, p_iActualCount);
}

template<class LOCK>
inline bool t_safeFile<LOCK>::Write(byte *p_buf, int p_iWriteCount, 
	 int &p_iActualCount)
{
	return m_file.Write(p_buf, p_iWriteCount, p_iActualCount);
}

template<class LOCK>
inline bool t_safeFile<LOCK>::Flush(void)
{
	return m_file.Flush();
}

template<class LOCK>
inline bool t_safeFile<LOCK>::Seek(uint p_pos, int p_iWhence)
{
	return m_file.Seek(p_pos, p_iWhence);
}

template<class LOCK>
inline uint t_safeFile<LOCK>::GetCurPos(void)
{
	return m_file.GetCurPos();
}

template<class LOCK>
inline t_safeFile<LOCK>::~t_safeFile()
{
	Close();
}
