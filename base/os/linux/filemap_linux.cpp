#include "filemap_linux.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "path_linux.h"
#include "file_linux.h"
#include <errno.h>
#include "strconvert_linux.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

t_filemap::t_filemap()
{
	m_pAddr = NULL;
	m_iSize = 0;
	m_state = ec_uninitiated;
	m_iFileDesc = -1;
	m_szShmName[0] = 0;
	m_bIsCreator = false;
}

t_filemap::t_filemap(const wchar_t * p_pPathName, const wchar_t *p_pShmName)
{
	m_pAddr = NULL;
	m_iSize = 0;
	m_state = ec_uninitiated;
	m_iFileDesc = -1;
	m_szShmName[0] = 0;
	m_bIsCreator = false;
	Open(p_pPathName, p_pShmName);
}

t_filemap::t_filemap( const t_path & p_path , const wchar_t *p_pShmName)
{
	m_pAddr = NULL;
	m_iSize = 0;
	m_state = ec_uninitiated;
	m_iFileDesc = -1;
	m_szShmName[0] = 0;
	m_bIsCreator = false;
	Open(p_path, p_pShmName);
}

t_filemap::~t_filemap()
{
	Close();
}

bool t_filemap::Open(const wchar_t *p_pPathName, const wchar_t *p_pShmName)
{
	t_path path(p_pPathName);
	return Open(path, p_pShmName);
}

bool t_filemap::Open(const t_path & p_path, const wchar_t *p_pShmName)
{
	mode_t mode = umask(0);
	p_path.NormPath();
	char szPath[MAX_PATH_LEN] = {0};
	int iActualLen = MAX_PATH_LEN;
	bool bRet = t_strConverter::W2C( p_path.FullPath().c_str(), szPath,
						 iActualLen);
	if( bRet == false )
	{
		assert(false);
		return false ;
	}
	m_iFileDesc = open(szPath, O_RDWR, 0666);
	umask(mode);

	if( m_iFileDesc == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	bool ret = t_fileUtil::GetFileSize( p_path, m_iSize);
	if( ret == false )
	{
		printf("get file size failed.\n");
		return false ;
	}
	m_pAddr = mmap(NULL, m_iSize, PROT_READ | PROT_WRITE,
				   MAP_SHARED, m_iFileDesc, 0);
	if( m_pAddr == MAP_FAILED )
	{
		perror( strerror(errno) );
		return false ;
	}
	m_state = ec_mapped;
	return true;
}

bool t_filemap::Open(const wchar_t *p_pShmName, int p_iShmSize)
{
	assert( p_iShmSize > 0 && p_pShmName != NULL );
	memset(m_szShmName, 0, MAX_PATH_LEN);
	int iActualLen = MAX_PATH_LEN;
	bool bRet = t_strConverter::W2C(p_pShmName, m_szShmName,
									iActualLen);
	if( bRet == false )
	{
		assert(false);
		return false;
	}
	mode_t mask = umask(0);

	m_iFileDesc = shm_open(m_szShmName,O_RDWR | O_CREAT, 
						   S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if( m_iFileDesc == -1 )
	{
		umask(mask);
		perror( strerror(errno) );
		return false;
	}
	umask(mask);
	int iRet = ftruncate(m_iFileDesc, p_iShmSize);
	if( iRet == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	m_pAddr = mmap(NULL, p_iShmSize, PROT_READ|PROT_WRITE,
				   MAP_SHARED, m_iFileDesc, 0);
	if( m_pAddr == NULL )
	{
		perror( strerror(errno) );
		return false;
	}
	m_bIsCreator = true;
	m_iSize = p_iShmSize;
	m_state = ec_mapped;

	return true;
}

bool t_filemap::OpenNC(const wchar_t *p_pShmName)
{
	assert( p_pShmName != NULL );
	memset(m_szShmName, 0, MAX_PATH_LEN);
	int iActualLen = MAX_PATH_LEN;
	bool bRet = t_strConverter::W2C(p_pShmName, m_szShmName,
									iActualLen);
	if( bRet == false )
	{
		assert(false);
		return false;
	}
	mode_t mask = umask(0);

	m_iFileDesc = shm_open(m_szShmName,O_RDWR, 
						   S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if( m_iFileDesc == -1 )
	{
		umask(mask);
		perror( strerror(errno) );
		return false;
	}
	umask(mask);
	struct stat curStat;
	int iRet = fstat(m_iFileDesc, &curStat);
	if( iRet == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	m_pAddr = mmap(NULL, curStat.st_size, PROT_READ|PROT_WRITE,
				   MAP_SHARED, m_iFileDesc, 0);
	if( m_pAddr == NULL )
	{
		perror( strerror(errno) );
		return false;
	}
	m_bIsCreator = false;
	m_iSize = curStat.st_size;
	m_state = ec_mapped;
	
	return true;
}

bool t_filemap::Sync(void *p_pAddr, size_t p_length)
{
	if ( p_pAddr == NULL )
	{
		p_pAddr = m_pAddr;
		p_length = m_iSize;
	}
	int ret = msync(p_pAddr, p_length, MS_SYNC);
	if( ret == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	return true;
}

bool t_filemap::IsOpen()
{
	if( m_state == ec_mapped )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool t_filemap::Close()
{
	int ret = 0;
	if( m_pAddr != NULL )
	{
		ret = munmap(m_pAddr, m_iSize);
		if( ret == -1 )
		{
			perror( strerror(errno) );
			return false;
		}
		m_pAddr = NULL;
		m_iSize = 0;
	}
	if( m_szShmName[0] != 0 )
	{
		ret = shm_unlink(m_szShmName);
		if( ret == -1 )
		{
			perror( strerror(errno) );
			return false;
		}
	}
	if( m_iFileDesc != -1 )
	{
		ret = close(m_iFileDesc);
		if( ret == -1 )
		{
			perror( strerror(errno) );
			return false;
		}
		m_iFileDesc = -1;
	}
	m_bIsCreator = false;

	return true;
}
