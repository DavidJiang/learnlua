#include "file_linux.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <sys/sendfile.h>
#include <dirent.h>
#include "strconvert_linux.h"

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN 1024
#endif

t_file::t_file()
{
	m_iFileDesc = 0;
	m_openMode = ec_invalid;
	m_state = ec_uninitiated;
	m_szLastError = NULL;
}

t_file::t_file(const wchar_t *p_pPathName, int p_mode )
{
	m_szLastError = NULL;
	Open(p_pPathName, p_mode);
}

t_file::t_file(const t_path & p_path, int p_mode )
{
	m_szLastError = NULL;
	Open(p_path, p_mode);
}

t_file::~t_file()
{
	Close();
}

bool t_file::IsOpen(void)
{
	if( m_state == ec_opened )
	{
		return true;
	}
	return false;
}

bool t_file::Open(const wchar_t *p_pPathName, int p_mode )
{
	m_filePath = p_pPathName;
	m_openMode = p_mode;
	int iFlags = 0;
	int iMode = 0;
	m_filePath.NormPath();

	char szPath[MAX_PATH_LEN] = {0};
	int iActualLen = MAX_PATH_LEN;
	bool bRet = t_strConverter::W2C(m_filePath.FullPath().c_str(), szPath,
						 iActualLen);
	if( bRet == false )
	{
		return false ;
	}

	TranslateMode2ActualFlags( m_openMode, iFlags, iMode );

	mode_t mask = umask(0);
	if ( p_mode == ec_in )
	{
		m_iFileDesc = open( szPath, iFlags);
	}
	else
	{
		m_iFileDesc = open( szPath, iFlags, iMode);
	}
	if( m_iFileDesc == -1 )
	{
		printf("open file %s failed.\n", szPath);
		perror( strerror(errno) );
		return false;
	}
	else
	{
		m_state = ec_opened;
	}
	umask(mask);
	return true;
}

bool t_file::Open(const t_path & p_path, int p_mode )
{
	p_path.NormPath();
	return Open(p_path.FullPath().c_str(), p_mode);
}

void t_file::TranslateMode2ActualFlags( int p_mode, int &p_iFlags, int &p_iMode)
{
	if( p_mode == ec_in )
	{
		p_iFlags = O_RDONLY;
		p_iMode = 0;
	}
	else if ( p_mode == ec_out )
	{
		p_iFlags = O_WRONLY | O_CREAT;
		p_iMode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH;
	}
	else if( p_mode == (ec_in | ec_out) )
	{
		p_iFlags = O_RDWR | O_CREAT;
		p_iMode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH;
	}
	else if( p_mode == (ec_out | ec_app) )
	{
		p_iFlags = O_WRONLY | O_CREAT | O_APPEND;
		p_iMode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH;
	}
	else if( p_mode == (ec_in | ec_out | ec_app) )
	{
		p_iFlags = O_RDWR | O_CREAT | O_APPEND;
		p_iMode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH;
	}
	else if( p_mode == (ec_out | ec_trunc) )
	{
		p_iFlags = O_WRONLY | O_CREAT | O_TRUNC;
		p_iMode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH;
	}
	else if( p_mode == (ec_in | ec_out | ec_trunc) )
	{
		p_iFlags = O_RDWR | O_CREAT | O_TRUNC;
		p_iMode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH;
	}
	else
	{
		assert(false);
	}
}

bool t_file::Close(void)
{
	if( m_iFileDesc != -1 && m_state == ec_opened )
	{
		int iRet = close(m_iFileDesc);
		if( iRet == -1 )
		{
			perror( strerror(errno) );
			assert(false);
			return false;
		}
		else
		{
			m_iFileDesc = -1;
		}
	}
	m_state = ec_closed;
	return true;
}

bool t_file::Read(unsigned char *p_buf, int p_iReadCount, int &p_iActualCount)
{
	ssize_t ret = read(m_iFileDesc, p_buf, p_iReadCount);
	if( ret == -1 )
	{
		perror( strerror(errno) );
		p_iActualCount = 0;
		return false;
	}
	p_iActualCount = ret;

	return true;
}

bool t_file::Write(const unsigned char *p_buf, int p_iWriteCount, int &p_iActualCount)
{
	ssize_t ret = write(m_iFileDesc, p_buf, p_iWriteCount);
	if( ret == -1 )
	{
		perror( strerror(errno) );
		p_iActualCount = 0;
		return false;
	}
	p_iActualCount = ret;

	return true;
}

bool t_file::Flush(void)
{
	int ret = fsync(m_iFileDesc);
	if( ret == -1 )
	{
		return false;
	}
	return true;
}

bool t_file::Seek(unsigned int p_pos, int p_iWhence )
{
	int iWhence = 0;
	if( p_iWhence == ec_seekSet )
	{
		iWhence = SEEK_SET;
	}
	else if( p_iWhence == ec_seekCur )
	{
		iWhence = SEEK_CUR;
	}
	else if( p_iWhence == ec_seekEnd )
	{
		iWhence = SEEK_END;
	}
	else
	{
		assert( false );
		return false;
	}
	off_t ret = lseek(m_iFileDesc, p_pos, iWhence);
	if( ret == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	return true;
}

unsigned int t_file::GetCurPos(void)
{
	off_t offset = lseek(m_iFileDesc, 0, SEEK_CUR);

	return offset;
}

int t_file::GetSize()
{
        if( m_iFileDesc == 0 )
        {
                return -1;
        }
        struct stat fileStat;
        int ret = fstat(m_iFileDesc, &fileStat);
	if( ret == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	return fileStat.st_size;
}

bool t_fileUtil::FileExists(const t_path &p_path)
{
	//todo: find a better way.
	struct stat sb;
	p_path.NormPath();
	char szPath[MAX_PATH_LEN] = {0};
	int iActualLen = MAX_PATH_LEN;
	t_strConverter::W2C( p_path.FullPath().c_str(), szPath,
					iActualLen);

	int ret = stat( szPath, &sb);
	if( ret == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	return true;
}

bool t_fileUtil::FolderExists(const t_path &p_path)
{
	//todo verify
	struct stat sb;
	p_path.NormPath();
	char szPath[MAX_PATH_LEN] = {0};
	int iActualLen = MAX_PATH_LEN;
	t_strConverter::W2C( p_path.FullPath().c_str(), szPath,
		iActualLen);

	int ret = stat( szPath, &sb);
	if( ret == -1 )
	{
		perror( strerror(errno) );
		return false;
	}

	if( S_ISDIR(sb.st_mode) )
	{
		return true;
	}

	return true;
}

bool t_fileUtil::GetFileSize(const t_path &p_path, size_t &p_fileSize)
{
	struct stat sb;
	p_path.NormPath();
	char szPath[MAX_PATH_LEN] = {0};
	int iActualLen = MAX_PATH_LEN;
	t_strConverter::W2C( p_path.FullPath().c_str(), szPath,
					iActualLen);

	int ret = stat( szPath, &sb);
	if( ret == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	p_fileSize = sb.st_size;
	return true;
}

bool t_fileUtil::RemoveFile(const t_path &p_path)
{
	p_path.NormPath();
	char szPath[MAX_PATH_LEN] = {0};
	int iActualLen = MAX_PATH_LEN;
	t_strConverter::W2C( p_path.FullPath().c_str(), szPath,
					iActualLen);

	int ret = remove(szPath);
	if( ret == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	return true;
}

bool t_fileUtil::MakeDir(const t_path &p_path)
{
	//todo:change mode according the needs.
	int mode = S_IRWXU | S_IRWXG | S_IROTH;
	p_path.NormPath();
	char szPath[MAX_PATH_LEN] = {0};
	int iActualLen = MAX_PATH_LEN;
	t_strConverter::W2C( p_path.FullPath().c_str(), szPath,
						 iActualLen);

	int ret = mkdir( szPath, mode);
	if( ret == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	return true;
}

bool t_fileUtil::RemoveDir(const t_path &p_path)
{
	p_path.NormPath();
	char szPath[MAX_PATH_LEN] = {0};
	int iActualLen = MAX_PATH_LEN;
	t_strConverter::W2C( p_path.FullPath().c_str(), szPath,
						 iActualLen);

	int ret = rmdir( szPath );
	if( ret == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	return true;
}

bool t_fileUtil::CopyFile(const t_path &p_fromPath, const t_path &p_toPath)
{
	t_file fromFile(p_fromPath, t_file::ec_in);
	t_file toFile(p_toPath, t_file::ec_out);
	size_t count = 0;
	if( !GetFileSize(p_fromPath, count) )
	{
		return false;
	}
	ssize_t ret = sendfile( toFile.m_iFileDesc, fromFile.m_iFileDesc ,
							NULL, count);
	if( ret == -1 )
	{
		perror( strerror( errno ) );
		return false;
	}
	return true;
}

bool t_fileUtil::MoveFile(const t_path &p_fromPath, const t_path &p_toPath)
{
	p_fromPath.NormPath();
	char szFromPath[MAX_PATH_LEN] = {0};
	int iActualLen = MAX_PATH_LEN;
	t_strConverter::W2C( p_fromPath.FullPath().c_str(), szFromPath,
						 iActualLen);
	p_toPath.NormPath();
	char szToPath[MAX_PATH_LEN] = {0};
	t_strConverter::W2C( p_toPath.FullPath().c_str(), szToPath,
						 iActualLen);

	int ret = rename( szFromPath, szToPath );
	if( ret == -1 )
	{
		perror( strerror(errno) );
		return false;
	}
	return true;
}

bool t_fileUtil::GetTempPath( t_path &p_pathTemp )
{
	//todo
	p_pathTemp = L"/tmp";
	return true;
}

//
// t_fileEnum
//
t_fileEnum::t_fileEnum(const wchar_t *p_szDir, bool p_bRecursive, const wchar_t *p_szFilter)
{
	if( !p_szDir )
	{
		return;
	}

	EnumFiles(p_szDir, p_bRecursive, p_szFilter);
}

t_fileEnum::~t_fileEnum()
{
	m_vFiles.clear();
}

int t_fileEnum::GetFileCount()
{
	return (int)m_vFiles.size();
}

const wchar_t *t_fileEnum::GetFileName(int p_nIndex)
{
	if( p_nIndex < 0 || p_nIndex >= (int)m_vFiles.size() )
	{
		return 0;
	}

	return m_vFiles[p_nIndex].c_str();
}

void t_fileEnum::EnumFiles(const wchar_t *p_szDir, bool p_bRecursive, const wchar_t *p_szFilter)
{
	t_path path = p_szDir;
	int nBufLen = 260;
	char szDir[260] = {0};
	t_strConverter::W2C(path.FullPath().c_str(), szDir, nBufLen, t_strConverter::ec_gbk);
	DIR *pDir = opendir(szDir);
	if( !pDir )
	{
		return;
	}

	struct dirent *pEnt = 0;
	while ( (pEnt = readdir(pDir)) != 0 )
	{
		if( strcmp(pEnt->d_name, ".") == 0 || strcmp(pEnt->d_name, "..") == 0 )
		{
			continue;
		}

		char szFile[260] = {0};
		sprintf(szFile, "%s/%s", szDir, pEnt->d_name);
		wchar_t wszFile[260] = {0};
		nBufLen = 260;
		t_strConverter::C2W(szFile, strlen(szFile), wszFile, nBufLen);

		struct stat theStat;
		if( lstat(szFile, &theStat) >= 0 && S_ISDIR(theStat.st_mode) )
		{
			if( p_bRecursive )
			{
				EnumFiles(wszFile, p_bRecursive, p_szFilter);
			}
		}
		else
		{
			m_vFiles.push_back(wszFile);
		}
	}

	closedir(pDir);
}
